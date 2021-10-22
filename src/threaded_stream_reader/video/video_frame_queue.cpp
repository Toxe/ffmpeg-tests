#include "video_frame_queue.hpp"

#include <algorithm>

#include "video_frame.hpp"

void VideoFrameQueue::push(VideoFrame* frame)
{
    std::lock_guard<std::mutex> lock(mtx_);

    queue_.push_back(frame);
    std::sort(queue_.begin(), queue_.end(), [](const VideoFrame* left, const VideoFrame* right) { return left->timestamp_ < right->timestamp_; });
}

VideoFrame* VideoFrameQueue::pop(double playback_position)
{
    std::lock_guard<std::mutex> lock(mtx_);

    if (queue_.empty())
        return nullptr;

    if (queue_.front()->timestamp_ <= playback_position) {
        auto frame = queue_.front();
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
