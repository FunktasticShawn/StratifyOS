
if( ${SOS_BUILD_CONFIG} STREQUAL arm )
    set(SOURCES ${SOURCES}
      ${SOURCES_PREFIX}/microchip/sst25vf_share.c
      ${SOURCES_PREFIX}/microchip/sst25vf_ssp_tmr.c
      ${SOURCES_PREFIX}/microchip/sst25vf_ssp.c
      ${SOURCES_PREFIX}/microchip/sst25vf_tmr.c
      ${SOURCES_PREFIX}/microchip/sst25vf.c
      ${SOURCES_PREFIX}/microchip/sst25vf_local.h
      ${SOURCES_PREFIX}/cfifo.c
			${SOURCES_PREFIX}/devfifo.c
			${SOURCES_PREFIX}/device_fifo.c
			${SOURCES_PREFIX}/ffifo.c
      ${SOURCES_PREFIX}/fifo.c
			${SOURCES_PREFIX}/stream_ffifo.c
			${SOURCES_PREFIX}/i2s_ffifo.c
			${SOURCES_PREFIX}/i2s_spi_ffifo.c
			${SOURCES_PREFIX}/full.c
      ${SOURCES_PREFIX}/led_pio.c
      ${SOURCES_PREFIX}/led_pwm.c
      ${SOURCES_PREFIX}/null.c
      ${SOURCES_PREFIX}/random.c
			${SOURCES_PREFIX}/drive_cfi_local.h
			${SOURCES_PREFIX}/drive_cfi.c
			${SOURCES_PREFIX}/drive_sdspi_local.h
			${SOURCES_PREFIX}/drive_sdio.c
			${SOURCES_PREFIX}/drive_sdio_dma.c
			${SOURCES_PREFIX}/drive_sdspi.c
			${SOURCES_PREFIX}/drive_sdssp.c
			${SOURCES_PREFIX}/switchboard.c
			${SOURCES_PREFIX}/tty_uart.c
      ${SOURCES_PREFIX}/tty_usbbulk.c
      ${SOURCES_PREFIX}/uartfifo.c
      ${SOURCES_PREFIX}/urandom.c
      ${SOURCES_PREFIX}/usbfifo.c
      ${SOURCES_PREFIX}/zero.c
      PARENT_SCOPE)
  endif()
