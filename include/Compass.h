/*
 * Compass.h
 *
 *  Created on: Dec 3, 2014
 *      Author: benling
 */

#ifndef COMPASS_H_
#define COMPASS_H_

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

void CompassReadAcc(float* pfData);
void CompassReadMag (float* pfData);
void CompassConfig(void);



#endif /* COMPASS_H_ */
