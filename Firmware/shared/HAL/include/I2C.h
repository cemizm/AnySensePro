/*
 * I2C.h
 *
 *  Created on: 23.03.2016
 *      Author: cem
 */

#ifndef HAL_INCLUDE_I2C_H_
#define HAL_INCLUDE_I2C_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>

#include "Pin.h"
#include "DMA.h"

namespace HAL
{

class I2C
{
private:
	const uint32_t m_i2c;
	const rcc_periph_clken m_rcc_clock;
	const Pin& m_scl;
	const Pin& m_sda;
	const uint8_t m_alt_func_num;
	const DMA& m_rx_dma;
	const DMA& m_tx_dma;
public:
	const uint8_t NVIC_IRQn;

	I2C(uint32_t i2c, rcc_periph_clken rcc_clock, Pin& scl, Pin& sda, uint8_t alt_func_num, uint8_t nvic_irqn, DMA& rx_dma,
			DMA& tx_dma) :
			m_i2c(i2c), m_rcc_clock(rcc_clock), m_scl(scl), m_sda(sda), m_alt_func_num(alt_func_num), m_rx_dma(rx_dma), m_tx_dma(
					tx_dma), NVIC_IRQn(nvic_irqn)
	{
	}

	inline const DMA& GetRXDMA()
	{
		return m_rx_dma;
	}

	inline const DMA& GetTXDMA()
	{
		return m_tx_dma;
	}

	inline void Init(uint8_t pull_up_down) const
	{
		rcc_set_i2c_clock_hsi(m_i2c);
		rcc_periph_clock_enable(m_rcc_clock);

		m_scl.PowerUp();
		m_scl.ModeSetup(GPIO_MODE_AF, pull_up_down);
		m_scl.SetOutputOptions(GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ);
		m_scl.Alternate(m_alt_func_num);

		m_sda.PowerUp();
		m_sda.ModeSetup(GPIO_MODE_AF, pull_up_down);
		m_sda.SetOutputOptions(GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ);
		m_sda.Alternate(m_alt_func_num);
	}

	inline void Enable() const
	{
		i2c_peripheral_enable(m_i2c);
	}

	inline void Disable() const
	{
		i2c_peripheral_disable(m_i2c);
	}

	inline void Reset() const
	{
		rcc_periph_reset_pulse(rcc_periph_rst::RST_I2C2);
	}

	inline void EnableRxDma() const
	{
		i2c_enable_rxdma(m_i2c);
	}

	inline void DisableRxDma() const
	{
		i2c_disable_rxdma(m_i2c);
	}

	inline void EnableTxDma() const
	{
		i2c_enable_txdma(m_i2c);
	}

	inline void DisableTxDma() const
	{
		i2c_disable_txdma(m_i2c);
	}

	inline void EnableAnalogFilter() const
	{
		i2c_enable_analog_filter(m_i2c);
	}

	inline void DisableAnalogFilter() const
	{
		i2c_disable_analog_filter(m_i2c);
	}

	inline void SetDigitalFilter(uint8_t mask) const
	{
		uint32_t tmp = I2C_CR1(m_i2c);
		tmp &= ~I2C_CR1_DNF_MASK;
		tmp |= (mask & I2C_CR1_DNF_MASK);
		I2C_CR1(m_i2c) = tmp;
	}

	inline void EnableAckGeneration() const
	{
		I2C_CR2(m_i2c) &= ~I2C_CR2_NACK;
	}

	inline void DisableAckGeneration() const
	{
		I2C_CR2(m_i2c) |= I2C_CR2_NACK;
	}

	inline void EnableStrechClock() const
	{
		i2c_enable_stretching(m_i2c);
	}
	inline void DisableStrechClock() const
	{
		i2c_disable_stretching(m_i2c);
	}

	inline void EnableSlaveByteControl() const
	{
		I2C_CR1(m_i2c) |= I2C_CR1_SBC;
	}

	inline void DisableSlaveByteControl() const
	{
		I2C_CR1(m_i2c) &= ~I2C_CR1_SBC;
	}

	inline uint8_t GetAddressMatched() const
	{
		return (I2C_ISR(m_i2c) >> 17);
	}

	inline uint8_t IsReadTransferDirection() const
	{
		return (I2C_ISR(m_i2c) & I2C_ISR_DIR_READ) != 0;
	}

	inline void SetDefaultTiming() const
	{
		i2c_100khz_i2cclk8mhz(m_i2c);
	}

	inline void SetTiming(uint32_t timing) const
	{
		I2C_TIMINGR(m_i2c) = timing;
	}

	inline void SoftwareReset() const
	{
		/* Disable peripheral */
		Disable();

		/* Perform a dummy read to delay the disable of peripheral for minimum
		 3 APB clock cycles to perform the software reset functionality */
		*(volatile uint32_t *) (uint32_t) m_i2c;
		*(volatile uint32_t *) (uint32_t) m_i2c;

		/* Enable peripheral */
		Enable();
	}

	inline void Read(uint8_t addr, uint8_t reg, uint8_t size, uint8_t* data) const
	{
		read_i2c(m_i2c, addr, reg, size, data);
	}

#ifndef OnwAddress

	inline void SetOwnAddress1(uint16_t addr, uint8_t mode) const
	{
		if (mode == 1)
		{
			i2c_set_own_10bit_slave_address(m_i2c, addr);
		}
		else
		{
			i2c_set_own_7bit_slave_address(m_i2c, addr);
		}
	}

	inline void EnableOwnAddress1() const
	{
		I2C_OAR1(m_i2c) |= I2C_OAR1_OA1EN_ENABLE;
	}

	inline void DisableOwnAddress1() const
	{
		I2C_OAR1(m_i2c) &= ~I2C_OAR1_OA1EN_ENABLE;
	}

	inline void SetOwnAddress2(uint8_t addr, uint16_t mask) const
	{
		uint32_t tmp = 0;
		tmp |= (addr << 1) & I2C_CR2_SADD_7BIT_MASK;
		tmp |= mask;
		I2C_OAR2(m_i2c) = tmp;
	}

	inline void EnableOwnAddress2() const
	{
		I2C_OAR2(m_i2c) |= I2C_OAR2_OA2EN;
	}

	inline void DisableOwnAddress2() const
	{
		I2C_OAR2(m_i2c) &= ~I2C_OAR2_OA2EN;
	}

#endif

#ifndef Interrupt

	inline void EnableInterrupt(uint32_t interrupt) const
	{
		i2c_enable_interrupt(m_i2c, interrupt);
	}

	inline void DisableInterrupt(uint32_t interrupt) const
	{
		i2c_disable_interrupt(m_i2c, interrupt);
	}

	inline void EnableAddrMatchInterrupt() const
	{
		EnableInterrupt(I2C_CR1_DDRIE);
	}

	inline void DisableAddrMatchInterrupt() const
	{
		DisableInterrupt(I2C_CR1_DDRIE);
	}

	inline uint32_t GetInterruptSource(uint32_t interrupt) const
	{
		if (interrupt & 0x7E)
		{
			if ((I2C_CR1(m_i2c) & (interrupt & 0x7E)) == 0)
				return 0;
		}

		return I2C_ISR(m_i2c) & interrupt;
	}

	inline void ClearInterruptFlag(uint32_t interrupt) const
	{
		I2C_ICR(m_i2c) |= interrupt;
	}

#endif

#ifndef DMA

	inline void SendDma(const uint8_t* address, uint16_t size)
	{
		m_tx_dma.DisableChannel();

		m_tx_dma.SetMemoryAddress((uint32_t) address);
		m_tx_dma.SetNumerOfData(size);

		m_tx_dma.EnableChannel();
	}

	inline void ReadDma(const uint8_t* address, uint16_t size)
	{
		m_rx_dma.DisableChannel();

		m_rx_dma.SetMemoryAddress((uint32_t) address);
		m_rx_dma.SetNumerOfData(size);

		m_rx_dma.EnableChannel();
	}

	inline void SetupTXDMA()
	{
		m_tx_dma.EnableClock();
		m_tx_dma.ChannelReset();
		m_tx_dma.SetPeriperalSize(DMA_CCR_PSIZE_8BIT);
		m_tx_dma.SetMemorySize(DMA_CCR_MSIZE_8BIT);
		m_tx_dma.EnableMemoryIncrementMode();
		m_tx_dma.DisablePeripheralIncremtnMode();
		m_tx_dma.SetReadFromMemory();
		m_tx_dma.SetPeripheralAddress((uint32_t) &I2C_TXDR(m_i2c));

		m_tx_dma.EnableTransferCompleteInterrupt();

		EnableTxDma();
	}

	inline void SetupRXDMA()
	{
		m_rx_dma.EnableClock();
		m_rx_dma.ChannelReset();
		m_rx_dma.SetPeriperalSize(DMA_CCR_PSIZE_8BIT);
		m_rx_dma.SetMemorySize(DMA_CCR_MSIZE_8BIT);
		m_rx_dma.EnableMemoryIncrementMode();
		m_rx_dma.DisablePeripheralIncremtnMode();
		m_rx_dma.SetReadFromPeripheral();
		m_rx_dma.SetPeripheralAddress((uint32_t) &I2C_RXDR(m_i2c));

		EnableRxDma();
	}

	inline void EnableRxDMAChannel()
	{
		m_rx_dma.EnableChannel();
	}

	inline void DisableRxDMAChannel()
	{
		m_rx_dma.DisableChannel();
	}

	inline void ClearTXDma()
	{
		m_tx_dma.ChannelReset();
	}

	inline void ClearRXDma()
	{
		m_rx_dma.ChannelReset();
	}

#endif

};

} /* namespace HAL */

#endif /* HAL_INCLUDE_I2C_H_ */
