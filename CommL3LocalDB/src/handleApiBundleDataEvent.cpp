#include "logging.h"
#include "higplat.h"
#include "CommL3Context.h"
#include "user_types.h"
#include <ctime>

void handleApiBundleDataEvent(CommL3Context &ctx, const char *value)
{
    // 解析API_BUNDLE_DATA_EVENT的值
    ApiBundleDataEvent bundleEvent = read_value<ApiBundleDataEvent>(value);

    spdlog::info("Processing API_BUNDLE_DATA_EVENT: order_no={}, item_no={}, bundle_no={}, flag={}",
                 bundleEvent.order_no.to_string(), bundleEvent.item_no.to_string(), bundleEvent.bundle_no.to_string(), bundleEvent.flag.to_string());


}