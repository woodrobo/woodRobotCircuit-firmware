/* 
 * File: QEI.h
 * Author: wood
 * Comments: QEI(Quadrature Encoder Interface) library
 * Revision history: ver1.00 2022/08/27
 */


#ifndef QEI_H
#define	QEI_H

typedef enum{
    QEI_COUNT_POLARITY_POSITIVE = 0,
    QEI_COUNT_POLARITY_NEGATIVE = 1
}QEI_COUNT_POLARITY;

void QEI_setup(QEI_COUNT_POLARITY polarity);
void QEI_write(uint32_t count);
uint32_t QEI_read();

#endif	/* QEI_H */

