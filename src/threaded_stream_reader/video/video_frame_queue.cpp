#include "video_frame_queue.hpp"

#include <algorithm>

#include "video_frame/video_frame.hpp"

void VideoFrameQueue::push(std::unique_ptr<VideoFrame> frame)
{
    std::lock_guard<std::mutex> lock(mtx_);

    queue_.push_back(std::move(frame));
    std::sort(queue_.begin(), queue_.end(), [](const std::unique_ptr<VideoFrame>& left, const std::unique_ptr<VideoFrame>& right) { return left->timestamp() < right->timestamp(); });
}

std::unique_ptr<VideoFrame> VideoFrameQueue::pop(double playback_position)
{
    std::lock_guard<std::mutex> lock(mtx_);

    if (queue_.empty())
        return nullptr;

    if (queue_.front()->timestamp() <= playback_position) {
        auto frame = std::move(queue_.front());
        queue_.erase(queue_.begin());
        return frame;
    } else {
        return nullptr;
    }
}

std::size_t VideoFrameQueue::size()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return queue_.size();
}

bool VideoFrameQueue::empty()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return queue_.empty();
}

bool VideoFrameQueue::full()
{
    std::lock_guard<std::mutex> lock(mtx_);
    return queue_.size() >= max_queue_size_;
}
