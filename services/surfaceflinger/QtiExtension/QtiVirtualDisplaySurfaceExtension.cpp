/* Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */
// #define LOG_NDEBUG 0
#include "QtiVirtualDisplaySurfaceExtension.h"

#include <log/log.h>

namespace android::surfaceflingerextension {

QtiDisplaySurfaceExtensionIntf* qtiCreateDisplaySurfaceExtension(bool isVirtual,
                                                                 VirtualDisplaySurface* vds,
                                                                 bool secure, uint64_t sinkUsage) {
    if (isVirtual) {
        ALOGV("Creating a new QtiVirtualDisplaySurfaceExtension");
        return new QtiVirtualDisplaySurfaceExtension(vds, secure, sinkUsage);
    }

    // Create a QtiFramebufferSurfaceExtension for real displays
    ALOGV("Creating a dummy QtiVirtualDisplaySurfaceExtension");
    return new QtiVirtualDisplaySurfaceExtension();
}

QtiVirtualDisplaySurfaceExtension::QtiVirtualDisplaySurfaceExtension(VirtualDisplaySurface* vds,
                                                                     bool secure,
                                                                     uint64_t sinkUsage)
      : mQtiVDS(vds) {
    if (!mQtiVDS) {
        ALOGW("Passed an invalid pointer to VirtualDisplaySurface");
    }

    mQtiSecure = secure;
    mQtiSinkUsage |= (GRALLOC_USAGE_HW_COMPOSER | sinkUsage);
    ALOGV("Successfully created QtiVDSExtension %p isSecure %d mQtiSinkUsage 0x%" PRIx64, mQtiVDS,
          mQtiSecure, mQtiSinkUsage);
}

uint64_t QtiVirtualDisplaySurfaceExtension::qtiSetOutputUsage() {
    return qtiSetOutputUsage(mQtiSinkUsage);
}

/* Helper to update the output usage when the display is secure */
uint64_t QtiVirtualDisplaySurfaceExtension::qtiSetOutputUsage(uint64_t flag) {
    uint64_t mOutputUsage = mQtiSinkUsage;

    if (mQtiSecure && (mOutputUsage & GRALLOC_USAGE_HW_VIDEO_ENCODER)) {
        /*TODO: Currently, the framework can only say whether the display
         * and its subsequent session are secure or not. However, there is
         * no mechanism to distinguish the different levels of security.
         * The current solution assumes WV L3 protection.
         */
        mOutputUsage |= GRALLOC_USAGE_PROTECTED;
    }

    ALOGV("QtiVirtualDisplaySurfaceExtension::%s, return mOutputUsage 0x%" PRIx64, __func__,
          mOutputUsage);
    return mOutputUsage;
}

uint64_t QtiVirtualDisplaySurfaceExtension::qtiExcludeVideoFromScratchBuffer(std::string source,
                                                                             uint64_t usage) {
    uint64_t usageFlag = usage;
    // Exclude video encoder usage flag from scratch buffer usage flags.
    usageFlag |= GRALLOC_USAGE_HW_FB;
    usageFlag &= ~(GRALLOC_USAGE_HW_VIDEO_ENCODER);
    ALOGV("%s(%s): updated scratch buffer usage flags=%#" PRIx64, __func__, source.c_str(),
          usageFlag);

    return usageFlag;
}

} // namespace android::surfaceflingerextension
