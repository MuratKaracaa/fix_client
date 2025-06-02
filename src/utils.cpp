#include "utils.h"
#include "absl/time/time.h"
#include "absl/time/clock.h"

int64_t Utils::convert_iso_timestamp_to_unix_timestamp(const std::string &iso_timestamp)
{
    absl::Time time;
    if (absl::ParseTime(absl::RFC3339_full, iso_timestamp, &time, nullptr))
    {
        return absl::ToUnixSeconds(time);
    }
    return absl::ToUnixSeconds(absl::Now());
}