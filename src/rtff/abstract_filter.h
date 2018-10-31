#ifndef RTFF_ABSTRACT_FILTER_H_
#define RTFF_ABSTRACT_FILTER_H_

#include <complex>
#include <memory>
#include <system_error>
#include <vector>

#include "rtff/buffer/audio_buffer.h"

namespace rtff {

class MultichannelOverlapRingBuffer;
class MultichannelRingBuffer;
class FilterImpl;

/**
 * @brief Base class of frequential filters.
 * Feed raw audio data and process them in the time frequency domain
 */
class AbstractFilter {
 public:
  AbstractFilter();
  virtual ~AbstractFilter();
  /**
   * @brief Initialize the filter
   * @param channel_count: the number of channel of the input signal
   * @param fft_size: the length in samples of the fourier transform window.
   * @param overlap: the number of samples that will be kept between each
   * window.
   * @param err: an error code that gets set if something goes wrong
   */
  void Init(uint8_t channel_count, uint32_t fft_size, uint32_t overlap,
            std::error_code& err);

  /**
   * @brief Initialize the filter with default stft parameters
   * @param channel_count: the number of channel of the input signal
   * @param err: an error code that gets set if something goes wrong
   */
  void Init(uint8_t channel_count, std::error_code& err);

  /**
   * @brief define the block size
   * @note the block size correspond to the number of frames contained in each
   * AudioBuffer sent to filter using the ProcessBlock function
   * @param value: the block size
   */
  void set_block_size(uint32_t value);

  /**
   * @brief Process a buffer
   * @note the buffer should have the same channel_count and its frame_number
   * should be equal to the filter block_size
   * @param buffer: the data
   */
  virtual void ProcessBlock(AudioBuffer* buffer);

  /**
   * @brief Acccess the number of frame of latency generated by the filter
   * @note Due to fourier transform computation, a filter most usually creates
   * latency. It depends on the block size, overlap and fft size.
   * @return The latency generated by the filter in frames.
   */
  virtual uint32_t FrameLatency() const;

  /**
   * @return the fft size in samples
   */
  uint32_t fft_size() const;
  /**
   * @return the overlap in samples
   */
  uint32_t overlap() const;
  /**
   * @return the hop size in sample
   */
  uint32_t hop_size() const;
  /**
   * @return the window size in samples
   * @note this value will be the same as the fft size
   */
  uint32_t window_size() const;
  /**
   * @return the block size
   * @see set_block_size
   */
  uint32_t block_size() const;
  /**
   * @return the number of channel of the input signal
   */
  uint8_t channel_count() const;

 protected:
  /**
   * @brief function called at the end of the initialization process.
   * @note Override this to initialize custom member in child classes
   */
  virtual void PrepareToPlay();

  /**
   * @brief Process a frequential buffer.
   * @note that function is called by the ProcessBlock function. It shouldn't be
   * called on its own
   * Override this function to design your filter
   */
  virtual void ProcessTransformedBlock(std::vector<std::complex<float>*> data,
                                       uint32_t size) = 0;

 private:
  void InitBuffers();

  uint32_t fft_size_;
  uint32_t overlap_;
  uint32_t block_size_;
  uint8_t channel_count_;
  std::shared_ptr<MultichannelOverlapRingBuffer> input_buffer_;
  std::shared_ptr<MultichannelRingBuffer> output_buffer_;

  std::shared_ptr<FilterImpl> impl_;

  class Impl;
  std::shared_ptr<Impl> buffers_;
};

}  // namespace rtff

#endif  // RTFF_ABSTRACT_FILTER_H_
