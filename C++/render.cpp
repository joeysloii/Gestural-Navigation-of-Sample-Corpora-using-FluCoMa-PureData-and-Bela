#include <Bela.h> // Bela Core Library
#include <libraries/BelaLibpd/BelaLibpd.h> // Communication between C++ and PD

// Linux libraries used to communicate with MPU6050 over I2C
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include <thread> // Runs IMU on a separate thread, prevents slow I2C interrupting DSP
#include <atomic> // For sharing data between threads

#include "MadgwickAHRS.h" // Madgwick filter converts raw IMU data into orientation values

// Data structure for storing IMU values
struct IMUData {
	float ax, ay, az; // Accelerometer
	float gx, gy, gz; // Gyroscope

    float pitch; // Orientation about X
    float roll; // Orientation about Y
    float yaw; // Orientation about Z

	float linX; // Linear acceleration X after removing gravity
    float linY; // Linear acceleration Y after removing gravity
    float linZ; // Linear acceleration Z after removing gravity
};

IMUData imu; // Latest sensor value
std::atomic<bool> newData(false); // Indicates that new IMU data is available
std::atomic<bool> running(true); // Controls when IMU thread is running

std::thread imuThread; // Separate thread for reading IMU

int i2c_fd = -1; // I2C File descriptor
const char* DEVICE = "/dev/i2c-1"; // Linux path to the I2C bus connected to MPU6050
const int MPU_ADDR = 0x68; // I2C address of MPU6050

Madgwick filter;

const float sampleRate = 100.0f;

// Calibration Offsets for Accelerometer
float accelBiasX = 0;
float accelBiasY = 0;
float accelBiasZ = 0;

// Calibration Offsets for Gyroscope
float gyroBiasX = 0;
float gyroBiasY = 0;
float gyroBiasZ = 0;

// Writes a value to a register on MPU6050
void writeReg(uint8_t reg, uint8_t val)
{
	uint8_t buf[2] = {reg, val};
	write(i2c_fd, buf, 2);
}

// MPU reads bytes from specified register
bool readBytes(uint8_t reg, uint8_t* buf, int len)
{
	write(i2c_fd, &reg, 1);
	return read(i2c_fd, buf, len) == len;
}

// IMU Thread
void imuLoop()
{
	uint8_t data[14];

	while(running) // Continuously read until stop
	{
		if(readBytes(0x3B, data, 14))
		{
            // Reading raw bytes from sensor
			int16_t rawAx = (data[0] << 8) | data[1];
			int16_t rawAy = (data[2] << 8) | data[3];
			int16_t rawAz = (data[4] << 8) | data[5];
			
			int16_t rawGx = (data[8] << 8) | data[9];
			int16_t rawGy = (data[10] << 8) | data[11];
			int16_t rawGz = (data[12] << 8) | data[13];
			
            // Converting raw accelerometer values into units of gravity
			float ax = ((float)rawAx - accelBiasX) / 16384.0f; // 16384 counts = 1g
			float ay = ((float)rawAy - accelBiasY) / 16384.0f;
			float az = ((float)rawAz) / 16384.0f;
			
            // Converts raw gyroscope values into degrees per second
			float gx = ((float)rawGx - gyroBiasX) / 131.0f;
			float gy = ((float)rawGy - gyroBiasY) / 131.0f;
			float gz = ((float)rawGz - gyroBiasZ) / 131.0f;
						
			// Update Madgwick filter
			filter.updateIMU( // Estimates orientation by combining Accelerometer/Gyro
			    gx,
			    gy,
			    gz,
			    ax,
			    ay,
			    az
			);

			// Get Madgwick quaternion so that gravity can be removed from acceleration
			float q0 = filter.getQ0();
			float q1 = filter.getQ1();
			float q2 = filter.getQ2();
			float q3 = filter.getQ3();
			
			// Gravity vector in sensor coordinates. Calculates direction of gravity in relation to the sensor's coordinate system.
			float gravityX = 2.0f * (q1*q3 - q0*q2);
			float gravityY = 2.0f * (q0*q1 + q2*q3);
			float gravityZ = q0*q0 - q1*q1 - q2*q2 + q3*q3;
			
			// Remove gravity, leaves only "true" movement
			imu.linX = ax - gravityX;
			imu.linY = ay - gravityY;
			imu.linZ = az - gravityZ;
			
			// Orientation
			imu.pitch = filter.getPitch();
			imu.roll  = filter.getRoll();
			imu.yaw   = filter.getYaw();
			
			// Keep useful sensor values
			imu.ax = ax;
			imu.ay = ay;
			imu.az = az;
			
			imu.gx = gx;
			imu.gy = gy;
			imu.gz = gz;

			newData.store(true); // Signals that new IMU data is available
		}

        // Wait 10ms
		usleep(10000);   // Matches 100 Hz sample rate
	}
}

// ----------------------------------------------------------

void Bela_userSettings(BelaInitSettings *settings)
{
	settings->uniformSampleRate = 1;
	settings->interleave = 0;
	settings->analogOutputsPersist = 0;
}

// ----------------------------------------------------------

bool setup(BelaContext *context, void *userData)
{
	i2c_fd = open(DEVICE, O_RDWR); // Open I2C bus
	if(i2c_fd < 0)
	{
		return false;
	}

	if(ioctl(i2c_fd, I2C_SLAVE, MPU_ADDR) < 0) // Selects MPU6050
	{
		return false;
	}
	
	// Wake the MPU6050
	writeReg(0x6B, 0x00);

	writeReg(0x1A, 0x03); // DLPF ~44Hz - reduces noise
	writeReg(0x1B, 0x00); // gyro mode ±250 deg/s
	writeReg(0x1C, 0x00); // accel mode ±2g

	if(!BelaLibpd_setup(context, userData, {}))
		return false;

    // Collect 2000 samples while stationary (for calibration)
	const int samples = 2000;

	for(int i = 0; i < samples; i++)
	{
		uint8_t data[14];
	
		if(readBytes(0x3B, data, 14))
		{
			int16_t rawAx = (data[0] << 8) | data[1];
			int16_t rawAy = (data[2] << 8) | data[3];
			int16_t rawAz = (data[4] << 8) | data[5];
	
			int16_t rawGx = (data[8] << 8) | data[9];
			int16_t rawGy = (data[10] << 8) | data[11];
			int16_t rawGz = (data[12] << 8) | data[13];
	
            // Biases accumulate after every reading
			accelBiasX += rawAx;
			accelBiasY += rawAy;
			accelBiasZ += rawAz;
	
			gyroBiasX += rawGx;
			gyroBiasY += rawGy;
			gyroBiasZ += rawGz;
		}
	
		usleep(5000);
	}
	
    // Find averages - treated as sensor offsets
	accelBiasX /= samples;
	accelBiasY /= samples;
	accelBiasZ /= samples;
	
	gyroBiasX /= samples;
	gyroBiasY /= samples;
	gyroBiasZ /= samples;

	// Start Magdwick filter
	filter.begin(sampleRate);

    // Start separate IMU thread
	imuThread = std::thread(imuLoop);

	return true;
}

// ----------------------------------------------------------

// Runs during audio processing
void render(BelaContext *context, void *userData)
{
	// Run Pd
	BelaLibpd_render(context, userData);

    // Check for new IMU data
	if(newData.exchange(false))
	{
		libpd_start_message(6); // Message containing IMU data
		
        // Append Pitch/Roll/Yaw/Acceleration to message
		libpd_add_float(imu.pitch);
		libpd_add_float(imu.roll);
		libpd_add_float(imu.yaw);
		
		libpd_add_float(imu.linX);
		libpd_add_float(imu.linY);
		libpd_add_float(imu.linZ);

        // Send as "imu"
		libpd_finish_list("imu");
	}
}

// ----------------------------------------------------------

void cleanup(BelaContext *context, void *userData)
{
	running = false; // Stop IMU thread

    // Wait until thread has stopeed
	if(imuThread.joinable())
		imuThread.join();

    // Close I2C
	if(i2c_fd >= 0)
		close(i2c_fd);

    // Releases PD
	BelaLibpd_cleanup(context, userData);
}
