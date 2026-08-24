#pragma once

#include "LocalStream.h"
#include "Channel.h"

class StreamAtPoint : public LocalStream {
	StreamAtPoint() = delete;
	StreamAtPoint(const StreamAtPoint&) = delete;
	StreamAtPoint(StreamAtPoint&&) = delete;
	StreamAtPoint& operator =(const StreamAtPoint&) = delete;
	StreamAtPoint& operator =(StreamAtPoint&&) = delete;

public:
	explicit StreamAtPoint(uint32_t color, std::string name,
			float distance, const sa::CVector& position) noexcept;

	~StreamAtPoint() noexcept = default;

public:
	void SetPosition(const sa::CVector& position) noexcept;

private:
	void OnChannelCreate(const Channel& channel) noexcept override;

private:
	sa::CVector position;
};

using StreamAtPointPtr = std::unique_ptr<StreamAtPoint>;
#define MakeStreamAtPoint std::make_unique<StreamAtPoint>
