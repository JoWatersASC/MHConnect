#pragma once

#include "common.h"

namespace osf
{

inline std::mutex send_mtx;
inline std::mutex recv_mtx;

using lock = std::unique_lock<std::mutex>;
using lockg = std::lock_guard<std::mutex>;

}