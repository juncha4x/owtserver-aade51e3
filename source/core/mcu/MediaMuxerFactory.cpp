/*
 * Copyright 2015 Intel Corporation All Rights Reserved. 
 * 
 * The source code contained or described herein and all documents related to the 
 * source code ("Material") are owned by Intel Corporation or its suppliers or 
 * licensors. Title to the Material remains with Intel Corporation or its suppliers 
 * and licensors. The Material contains trade secrets and proprietary and 
 * confidential information of Intel or its suppliers and licensors. The Material 
 * is protected by worldwide copyright and trade secret laws and treaty provisions. 
 * No part of the Material may be used, copied, reproduced, modified, published, 
 * uploaded, posted, transmitted, distributed, or disclosed in any way without 
 * Intel's prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual 
 * property right is granted to or conferred upon you by disclosure or delivery of 
 * the Materials, either expressly, by implication, inducement, estoppel or 
 * otherwise. Any license under such intellectual property rights must be express 
 * and approved by Intel in writing.
 */

#include "MediaMuxerFactory.h"
#include "media/RTSPMuxer.h"
#include "media/MediaRecorder.h"

#include <boost/property_tree/json_parser.hpp>

namespace mcu {

DEFINE_LOGGER(MediaMuxerFactory, "mcu.MediaMuxerFactory");

std::map<std::string, boost::shared_ptr<woogeen_base::MediaMuxer>> MediaMuxerFactory::m_muxers;

woogeen_base::MediaMuxer* MediaMuxerFactory::getMediaMuxer(const std::string& id, const std::string& configParam)
{
    if (id != "") {
        std::map<std::string, boost::shared_ptr<woogeen_base::MediaMuxer>>::iterator it = m_muxers.find(id);
        if (it != m_muxers.end()) {
            return m_muxers[id].get();
        }

        if (configParam != "" && configParam != "undefined") {
            boost::property_tree::ptree pt;
            std::istringstream is(configParam);
            boost::property_tree::read_json(is, pt);
            const std::string url = pt.get<std::string>("url", "");
            int snapshotInterval = pt.get<int>("interval", -1);

            // Create a new MediaMuxer
            woogeen_base::MediaMuxer* muxer = nullptr;
            if (url.compare(0, 7, "rtsp://") == 0) {
                muxer = new RTSPMuxer(url);
            } else {
                muxer = new MediaRecorder(url, snapshotInterval);
            }

            m_muxers[id] = boost::shared_ptr<woogeen_base::MediaMuxer>(muxer);

            return muxer;
        }
    }

    ELOG_DEBUG("No media muxer found or created with id: %s.", id.c_str());
    return nullptr;
}

woogeen_base::MediaMuxer* MediaMuxerFactory::getMediaMuxer(const std::string& id)
{
    if (id != "") {
        std::map<std::string, boost::shared_ptr<woogeen_base::MediaMuxer>>::iterator it = m_muxers.find(id);
        if (it != m_muxers.end()) {
            return m_muxers[id].get();
        }
    }

    ELOG_DEBUG("No media muxer can be found with id: %s.", id.c_str());
    return nullptr;
}

bool MediaMuxerFactory::removeMediaMuxer(const std::string& id)
{
    std::map<std::string, boost::shared_ptr<woogeen_base::MediaMuxer>>::iterator it = m_muxers.find(id);
    if (it != m_muxers.end()) {
        m_muxers.erase(it);
        return true;
    }

    ELOG_DEBUG("No media muxer to be removed with id: %s.", id.c_str());
    return false;
}

} /* namespace mcu */