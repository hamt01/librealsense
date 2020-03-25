// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2020 Intel Corporation. All Rights Reserved.

#include <mutex>
#include <chrono>
#include <vector>
#include <iterator>
#include <cstddef>

#include "device.h"
#include "context.h"
#include "image.h"
#include "metadata-parser.h"

#include "fa-factory.h"
#include "fa-private.h"
#include "fa-device.h"
//#include "ds5-options.h"
//#include "ds5-timestamp.h"
#include "sync.h"

namespace librealsense
{

    // F450
    class rs450_device : public fa_device
    {
    public:
        rs450_device(const std::shared_ptr<context>& ctx,
            const std::vector<platform::uvc_device_info>& uvc_infos,
            const platform::backend_device_group& group,
            bool register_device_notifications)
            : device(ctx, group, register_device_notifications),
            fa_device(ctx, uvc_infos, group, register_device_notifications){}

        std::vector<tagged_profile> get_profiles_tags() const override
        {
            std::vector<tagged_profile> tags;
            auto usb_spec = get_usb_spec();
            if (usb_spec >= platform::usb3_type || usb_spec == platform::usb_undefined)
            {
                tags.push_back({ RS2_STREAM_INFRARED, 0, 720, 720, RS2_FORMAT_RGB8, 30, profile_tag::PROFILE_TAG_SUPERSET | profile_tag::PROFILE_TAG_DEFAULT });
            }
            else
            {
                tags.push_back({ RS2_STREAM_INFRARED, 1, 640, 480, RS2_FORMAT_Y8, 15, profile_tag::PROFILE_TAG_SUPERSET | profile_tag::PROFILE_TAG_DEFAULT });
            }
            return tags;
        };
    };

    std::shared_ptr<device_interface> fa_info::create(std::shared_ptr<context> ctx,
                                                       bool register_device_notifications) const
    {
        using namespace fa;
        return std::make_shared<rs450_device>(ctx, _uvcs, this->get_device_data(), register_device_notifications);
    }

    std::vector<std::shared_ptr<device_info>> fa_info::pick_fa_devices(
        std::shared_ptr<context> ctx,
        const std::vector<platform::uvc_device_info>& uvc_devices)
    {
        std::vector<std::shared_ptr<device_info>> list;
        auto groups = group_devices_by_unique_id(uvc_devices);

        for (auto&& g : groups)
        {
            if (g.front().vid == fa::RS450_VID)
                list.push_back(std::make_shared<fa_info>(ctx, g));
        }
        return list;
    }


    inline std::shared_ptr<matcher> create_composite_matcher(std::vector<std::shared_ptr<matcher>> matchers)
    {
        return std::make_shared<timestamp_composite_matcher>(matchers);
    }

}
