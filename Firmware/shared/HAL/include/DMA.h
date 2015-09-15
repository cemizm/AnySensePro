/*
 * DMA.h
 *
 *  Created on: 14.09.2015
 *      Author: cem
 */

#ifndef HAL_INCLUDE_DMA_H_
#define HAL_INCLUDE_DMA_H_

#include <libopencm3/stm32/dma.h>

namespace HAL
{

class DMA
{
private:
	const uint32_t m_dma;
	const uint8_t m_channel;
	const rcc_periph_clken m_rcc;
public:
	const uint8_t NVIC_IRQn;

	DMA(uint32_t dma, uint8_t channel, uint8_t nvic_irqn, rcc_periph_clken rcc) :
			m_dma(dma), m_channel(channel), m_rcc(rcc), NVIC_IRQn(nvic_irqn)
	{
	}

	inline void EnableClock() const
	{
		rcc_periph_clock_enable(m_rcc);
	}

	inline void ChannelReset() const
	{
		dma_channel_reset(m_dma, m_channel);
	}

	inline void ClearInterruptFlags(uint32_t interrupts) const
	{
		dma_clear_interrupt_flags(m_dma, m_channel, interrupts);
	}

	inline uint8_t GetInterruptFlag(uint32_t interrupts) const
	{
		return dma_get_interrupt_flag(m_dma, m_channel, interrupts) ? 1 : 0;
	}

	inline void EnableMem2MemMode() const
	{
		dma_enable_mem2mem_mode(m_dma, m_channel);
	}

	inline void SetPriority(uint32_t priority) const
	{
		dma_set_priority(m_dma, m_channel, priority);
	}

	inline void SetMemorySize(uint32_t memSize) const
	{
		dma_set_memory_size(m_dma, m_channel, memSize);
	}

	inline void SetPeriperalSize(uint32_t peripherialSize) const
	{
		dma_set_peripheral_size(m_dma, m_channel, peripherialSize);
	}

	inline void EnableMemoryIncrementMode() const
	{
		dma_enable_memory_increment_mode(m_dma, m_channel);
	}

	inline void DisableMemoryIncrementMode() const
	{
		dma_disable_memory_increment_mode(m_dma, m_channel);
	}

	inline void EnablePeripheralIncrementMode() const
	{
		dma_enable_peripheral_increment_mode(m_dma, m_channel);
	}

	inline void DisablePeripheralIncremtnMode() const
	{
		dma_disable_peripheral_increment_mode(m_dma, m_channel);
	}

	inline void EnableCircularMode() const
	{
		dma_enable_circular_mode(m_dma, m_channel);
	}

	inline void SetReadFromPeripheral() const
	{
		dma_set_read_from_peripheral(m_dma, m_channel);
	}

	inline void SetReadFromMemory() const
	{
		dma_set_read_from_memory(m_dma, m_channel);
	}

	inline void EnableTransferErrorInterrupt() const
	{
		dma_enable_transfer_error_interrupt(m_dma, m_channel);
	}

	inline void DisableTransferErrorInterrupt() const
	{
		dma_disable_transfer_error_interrupt(m_dma, m_channel);
	}

	inline void EnableHalfTransferInterrupt() const
	{
		dma_enable_half_transfer_interrupt(m_dma, m_channel);
	}

	inline void DisableHalfTransferInterrupt() const
	{
		dma_disable_half_transfer_interrupt(m_dma, m_channel);
	}

	inline void EnableTransferCompleteInterrupt() const
	{
		dma_enable_transfer_complete_interrupt(m_dma, m_channel);
	}

	inline void DisableTransferCompleteInterrupt() const
	{
		dma_disable_transfer_complete_interrupt(m_dma, m_channel);
	}

	inline void EnableChannel() const
	{
		dma_enable_channel(m_dma, m_channel);
	}

	inline void DisableChannel() const
	{
		dma_disable_channel(m_dma, m_channel);
	}

	inline void SetPeripheralAddress(uint32_t address) const
	{
		dma_set_peripheral_address(m_dma, m_channel, address);
	}

	inline void SetMemoryAddress(uint32_t address) const
	{
		dma_set_memory_address(m_dma, m_channel, address);
	}

	inline void SetNumerOfData(uint32_t number) const
	{
		dma_set_number_of_data(m_dma, m_channel, number);
	}

};

} /* namespace HAL */

#endif /* HAL_INCLUDE_DMA_H_ */
