void i2s_install(i2s_port_t I2S_PORT, int SAMPLES, int FREQUENCY)
{
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = FREQUENCY,
      .bits_per_sample = i2s_bits_per_sample_t(16),
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = SAMPLES,
      .use_apll = false};

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin(int I2S_WS, int I2S_SD, int I2S_SCK, i2s_port_t I2S_PORT)
{
  const i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_SCK,
      .ws_io_num = I2S_WS,
      .data_out_num = -1,
      .data_in_num = I2S_SD};

  i2s_set_pin(I2S_PORT, &pin_config);
}

void i2s_configure(int I2S_WS, int I2S_SD, int I2S_SCK, i2s_port_t I2S_PORT, int SAMPLES, int FREQUENCY)
{
  i2s_install(I2S_PORT, SAMPLES, FREQUENCY);
  i2s_setpin(I2S_WS, I2S_SD, I2S_SCK, I2S_PORT);
  i2s_start(I2S_PORT);
}
