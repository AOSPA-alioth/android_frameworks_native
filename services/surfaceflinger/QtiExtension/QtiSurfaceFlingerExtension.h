/* Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */
#pragma once

#ifndef QTI_DISPLAY_EXTENSION
#define QTI_DISPLAY_EXTENSION
#endif

#include "QtiSurfaceFlingerExtensionIntf.h"

#include <binder/IBinder.h>
#include <composer_extn_intf.h>
#include <list>

#include "../DisplayHardware/HWComposer.h"
#include "../DisplayHardware/PowerAdvisor.h"
#include "../SurfaceFlinger.h"
#include "QtiFeatureManager.h"
#include "QtiHWComposerExtension.h"
#include "QtiNullExtension.h"
#include "QtiPhaseOffsetsExtension.h"
#include "QtiPowerAdvisorExtension.h"
#include "QtiWorkDurationsExtension.h"

namespace composer {
class ComposerExtnIntf;
class ComposerExtnLib;
class FrameSchedulerIntf;
class DisplayExtnIntf;
} // namespace composer

namespace android::surfaceflingerextension {

class QtiSurfaceFlingerExtension : public QtiSurfaceFlingerExtensionIntf {
public:
    QtiSurfaceFlingerExtension();
    ~QtiSurfaceFlingerExtension();

    void qtiInit(SurfaceFlinger* flinger) override;
    QtiSurfaceFlingerExtensionIntf* qtiPostInit(
            android::impl::HWComposer& hwc, Hwc2::impl::PowerAdvisor* powerAdvisor,
            scheduler::VsyncConfiguration* vsyncConfig) override;
    void qtiSetVsyncConfiguration(scheduler::VsyncConfiguration* vsyncConfig) override;
    void qtiSetTid() override;
    bool qtiGetHwcDisplayId(const sp<DisplayDevice>& display, uint32_t* hwcDisplayId) override;
    void qtiHandlePresentationDisplaysEarlyWakeup(size_t updatingDisplays,
                                                  uint32_t layerStackId) override;
    bool qtiIsInternalPresentationDisplays() { return mQtiInternalPresentationDisplays; };
    bool qtiIsWakeUpPresentationDisplays() { return mQtiWakeUpPresentationDisplays; };
    void qtiResetEarlyWakeUp() override;
    void qtiSetDisplayExtnActiveConfig(uint32_t displayId, uint32_t activeConfigId) override;
    void qtiUpdateDisplayExtension(uint32_t displayId, uint32_t configId, bool connected) override;
    void qtiUpdateDisplaysList(sp<DisplayDevice> display, bool addDisplay) override;
    void qtiUpdateOnProcessDisplayHotplug(uint32_t hwcDisplayId, hal::Connection connection,
                                          PhysicalDisplayId id) override;
    void qtiUpdateOnComposerHalHotplug(hal::HWDisplayId hwcDisplayId,
                                       hal::Connection connection) override;
    void qtiUpdateInternalDisplaysPresentationMode() override;

    /*
     * Methods that call the FeatureManager APIs.
     */
    bool qtiIsExtensionFeatureEnabled(QtiFeature feature) override;

    /*
     * Methods used by SurfaceFlinger DisplayHardware.
     */
    status_t qtiSetDisplayElapseTime(
            std::chrono::steady_clock::time_point earliestPresentTime) const override;

    /*
     * Methods that call the DisplayExtension APIs.
     */
    void qtiSendCompositorTid() override;
    void qtiSendInitialFps(uint32_t fps) override;
    void qtiNotifyDisplayUpdateImminent() override;
    void qtiSetContentFps(uint32_t contentFps) override;
    void qtiSetEarlyWakeUpConfig(const sp<DisplayDevice>& display, hal::PowerMode mode) override;
    void qtiUpdateVsyncConfiguration() override;

    /*
     * Methods that call FrameScheduler APIs.
     */
    void qtiUpdateFrameScheduler() override;

    /*
     * Methods that call the IDisplayConfig APIs.
     */
    status_t qtiGetDebugProperty(string prop, string* value) override;
    status_t qtiIsSupportedConfigSwitch(const sp<IBinder>& displayToken, int config) override;

private:
    bool qtiIsInternalDisplay(const sp<DisplayDevice>& display);
    void qtiSetupDisplayExtnFeatures();

    SurfaceFlinger* mQtiFlinger = nullptr;
    composer::ComposerExtnIntf* mQtiComposerExtnIntf = nullptr;
    composer::DisplayExtnIntf* mQtiDisplayExtnIntf = nullptr;
    composer::FrameSchedulerIntf* mQtiFrameSchedulerExtnIntf = nullptr;
    QtiFeatureManager* mQtiFeatureManager = nullptr;
    QtiHWComposerExtension* mQtiHWComposerExtn = nullptr;
    QtiPowerAdvisorExtension* mQtiPowerAdvisorExtn = nullptr;
    QtiPhaseOffsetsExtension* mQtiPhaseOffsetsExtn = nullptr;
    QtiWorkDurationsExtension* mQtiWorkDurationsExtn = nullptr;

    bool mQtiEnabledIDC = false;
    bool mQtiInitVsyncConfigurationExtn = false;
    bool mQtiInternalPresentationDisplays = false;
    bool mQtiSendEarlyWakeUp = false;
    bool mQtiSentInitialFps = false;
    bool mQtiTidSentSuccessfully = false;
    bool mQtiWakeUpPresentationDisplays = false;
    int mQtiRETid = 0;
    int mQtiSFTid = 0;
    uint32_t mQtiCurrentFps = 0;

    std::list<sp<DisplayDevice>> mQtiDisplaysList = {};
    std::mutex mQtiEarlyWakeUpMutex;
    std::unordered_map<float, int64_t> mQtiAdvancedSfOffsets;
    std::unordered_map<float, std::pair<int64_t, int64_t>> mQtiWorkDurationConfigsMap;
};

} // namespace android::surfaceflingerextension
