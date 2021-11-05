#include "video_frame_queue.hpp"

#include <algorithm>

#include "video_frame/video_frame.hpp"

void VideoFrameQueue::push(std::unique_ptr<VideoFrame> frame)
{
    queue_.push_back(std::move(frame));
    std::sort(queue_.begin(), queue_.end(), [](const std::unique_ptr<VideoFrame>& left, const std::unique_ptr<VideoFrame>& right) { return left->timestamp() < right->timestamp(); });
}

std::unique_ptr<VideoFrame> VideoFrameQueue::pop(double playback_position)
{
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

int VideoFrameQueue::size() const
{
    return static_cast<int>(queue_.size());
}

bool VideoFrameQueue::empty() const
{
    return queue_.empty();
}

bool VideoFrameQueue::full() const
{
    return queue_.size() >= max_queue_size_;
}
