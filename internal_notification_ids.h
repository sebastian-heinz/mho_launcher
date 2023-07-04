#ifndef MHO_LAUNCHER_INTERNAL_NOTIFICATION_IDS
#define MHO_LAUNCHER_INTERNAL_NOTIFICATION_IDS

#include <cstdint>

enum class MHGameEventID
{
    EnterInstance_f = 0x3F800000,
    GMCommand_f = 0x40000000,
    LevelLoading_f = 0x40400000,
    LevelLoaded_f = 0x40800000,
    SpawnLocalPlayer_f = 0x40A00000,
    LeaveInstanceReq_f = 0x40C00000,
    PlayerDialogueRsp_f = 0x40E00000,
    EnterInstance = 0x1,
    GMCommand = 0x2,
    LevelLoading = 0x3,
    LevelLoaded = 0x4,
    SpawnLocalPlayer = 0x5,
    LeaveInstanceReq = 0x6,
    PlayerDialogueRsp = 0x7,
    ShowSystemConfig = 0x8,
    AddCraft = 0x9,
    BeginCraft = 0xA,
    BeginLoot = 0xB,
    Resetconnected = 0xC,
    AutoLaunch = 0xD,
    RoleListResponse = 0xE,
    SelectRoleResponse = 0xF,
    GotoMap = 0x10,
    GotoMapResponse = 0x11,
    MoveToDest = 0x12,
    ChangeOrient = 0x13,
    MainInstanceEnterAgree = 0x14,
    MainInstanceSynOpt = 0x15,
    MainInstanceEnter = 0x16,
    FadingInStartNotify = 0x17,
    InstanceClearingStart = 0x18,
    InstanceClearingFinish = 0x19,
    WebPageDownloadNotify = 0x1A,
    KeyMapInputNotify = 0x1B,
    NewCreateRoleRsp = 0x1C,
    NewRoleListRsp = 0x1D,
    NewDeleteRoleRsp = 0x1E,
    NewSelecteRoleRsp = 0x1F,
    FangchengmiTimeNotify = 0x20,
    ChangeBullet = 0x21,
    GameStageChanged = 0x22,
    ChangeCharacterAvatar = 0x23,
    SelectCharacter = 0x24,
    DeleteRole = 0x25,
    CreateRole = 0x26,
    ResponeLineInfoRefresh = 0x27,
    RoleShow = 0x28,
    GuideStepStart = 0x29,
    GuideStepStop = 0x2A,
    UIGuideStepComplete = 0x2B,
    ShowOrHideDialogIcon = 0x2C,
    SplashMovieEsc = 0x2D,
    RequestSelectLine = 0x2E,
    RequestEntrustList = 0x2F,
    AcceptEntrustTask = 0x30,
    GiveUpEntrustTask = 0x31,
    EntrustSelHuntBox = 0x32,
    RequestAllEntrustList = 0x33,
    MinimapRegionState = 0x34,
    MinimapLinkState = 0x35,
    MinimapPointState = 0x36,
    RequestPaidBox = 0x37,
    TutorialStart = 0x38,
    TutorialActionExecuted = 0x39,
    UIWidgetShow = 0x3A,
    UIWidgetHide = 0x3B,
    AirShipState = 0x3C,
    PVPSelHuntBox = 0x3D,
    PetShopAvatarView = 0x3E,
    TrackViewEvent = 0x3F,
    ChangeCharacterAvatarWithGreatFace = 0x40,
    PvpOpenReadyPlayerInfoUI = 0x41,
    TempPvpLineupReq = 0x42,
    TempPvpQuitLineReq = 0x43,
    WeaponTalentSkillActivated = 0x44,
    WeaponTalentSkillUnactivated = 0x45,
    WeaponTalentSkillUpgraded = 0x46,
    WeaponTalentSkillSlotUnlock = 0x47,
    FarmAdvGatherCutSceneEnd = 0x48,
    OpenWeaponTutorialVideo = 0x49,
    CloseWeaponTutorialVideo = 0x4A,
    OpenWeaponTutorialClear = 0x4B,
    CloseWeaponTutorialClear = 0x4C,
    OpenCommonFunctionalList = 0x4D,
    CloseCommonFunctionalList = 0x4E,
    StartCommonFunctionByID = 0x4F,
    NotifyFGTutorialVideoClosed = 0x50,
    RequestOpenEquipShow = 0x51,
    ShowEquipShowItemByEvent = 0x52,
    FGNotifyUIShow_HideMouse = 0x53,
    NotifyFGWidgetUnloaded = 0x54,
    ActivityRequestSimpleData = 0x55,
    ActivityRequestDetailInfo = 0x56,
    MarqueenAddressUpdate = 0x57,
    ActivityRequestUniqueID = 0x58,
    OnResponseActivityResponseSimpleData = 0x59,
    OnResponseActivityResponseDetailInfo = 0x5A,
    OnResponseActivityResponseAcceptTask = 0x5B,
    OnResponseActivityResponsePickupItem = 0x5C,
    OnRoundSwfDownloadFinished = 0x5D,
    OnChangeTownInstance = 0x5E,
    OperationActivitySimpleDataReady = 0x5F,
    OperationActivityDataExecRsp = 0x60,
    OpenChargeInternalUrl = 0x61,
    S2CUpdateOperationActData = 0x62,
    S2CUpdateOperationItemCountsRefresh = 0x63,
    S2CUpdateOperationItemCountsRefreshData = 0x64,
    WeaponUnlockViewEnableSkillVideo = 0x65,
    WeaponUnlockPrework = 0x66,
    OperationActivityHasNewData = 0x67,
    ExitWindowDelay = 0x68,
    InstanceSelectQuickMatch = 0x69,
    InstanceSelectHubEnterLevel = 0x6A,
    InstanceCaptainSelectLevel = 0x6B,
    InstanceSelect_UnlockEliteLevelID = 0x6C,
    LevelUnlockNotify = 0x6D,
    SelectTutorialWeapon = 0x6E,
    WeaponTutorialStepUpdated = 0x6F,
    WeaponTutorialClearAllShortcut = 0x70,
    UpdateShortcutItem = 0x71,
    WeaponTutorialUpdateUIWeaponType = 0x72,
    SetTeachUIClickMode = 0x73,
    TeachUIClickedEvent = 0x74,
    LobbyChanged = 0x75,
    VipLevelNotify = 0x76,
    UIWidgetMinimize = 0x77,
    UIWidgetMaximize = 0x78,
    BeginChangeFace = 0x79,
    EndChangeFace = 0x7A,
    ChangeFaceResponse = 0x7B,
    PlayerBeautyAcountInfoNotify = 0x7C,
    MvMPetMonsterDeadNotify = 0x7D,
    ReturnTownNotify = 0x7E,
    ReturnChooseRoleNotify = 0x7F,
    SaveNetworkSequenceNotify = 0x80,
    OnLeaveCamp = 0x81,
    OnLeaveJumpArea = 0x82,
    OnLeaveFarm = 0x83,
    OnEnterFarm = 0x84,
    OnEnterCamp = 0x85,
};

inline const char* GetMHGameEventIDName(MHGameEventID event) {
    switch (event) {
    case MHGameEventID::EnterInstance_f:
        return "MHGameEventID::EnterInstance_f";
    case MHGameEventID::GMCommand_f:
        return "MHGameEventID::GMCommand_f";
    case MHGameEventID::LevelLoading_f:
        return "MHGameEventID::LevelLoading_f";
    case MHGameEventID::LevelLoaded_f:
        return "MHGameEventID::LevelLoaded_f";
    case MHGameEventID::SpawnLocalPlayer_f:
        return "MHGameEventID::SpawnLocalPlayer_f";
    case MHGameEventID::LeaveInstanceReq_f:
        return "MHGameEventID::LeaveInstanceReq_f";
    case MHGameEventID::PlayerDialogueRsp_f:
        return "MHGameEventID::PlayerDialogueRsp_f";
    case MHGameEventID::EnterInstance:
        return "MHGameEventID::EnterInstance";
    case MHGameEventID::GMCommand:
        return "MHGameEventID::GMCommand";
    case MHGameEventID::LevelLoading:
        return "MHGameEventID::LevelLoading";
    case MHGameEventID::LevelLoaded:
        return "MHGameEventID::LevelLoaded";
    case MHGameEventID::SpawnLocalPlayer:
        return "MHGameEventID::SpawnLocalPlayer";
    case MHGameEventID::LeaveInstanceReq:
        return "MHGameEventID::LeaveInstanceReq";
    case MHGameEventID::PlayerDialogueRsp:
        return "MHGameEventID::PlayerDialogueRsp";
    case MHGameEventID::ShowSystemConfig:
        return "MHGameEventID::ShowSystemConfig";
    case MHGameEventID::AddCraft:
        return "MHGameEventID::AddCraft";
    case MHGameEventID::BeginCraft:
        return "MHGameEventID::BeginCraft";
    case MHGameEventID::BeginLoot:
        return "MHGameEventID::BeginLoot";
    case MHGameEventID::Resetconnected:
        return "MHGameEventID::Resetconnected";
    case MHGameEventID::AutoLaunch:
        return "MHGameEventID::AutoLaunch";
    case MHGameEventID::RoleListResponse:
        return "MHGameEventID::RoleListResponse";
    case MHGameEventID::SelectRoleResponse:
        return "MHGameEventID::SelectRoleResponse";
    case MHGameEventID::GotoMap:
        return "MHGameEventID::GotoMap";
    case MHGameEventID::GotoMapResponse:
        return "MHGameEventID::GotoMapResponse";
    case MHGameEventID::MoveToDest:
        return "MHGameEventID::MoveToDest";
    case MHGameEventID::ChangeOrient:
        return "MHGameEventID::ChangeOrient";
    case MHGameEventID::MainInstanceEnterAgree:
        return "MHGameEventID::MainInstanceEnterAgree";
    case MHGameEventID::MainInstanceSynOpt:
        return "MHGameEventID::MainInstanceSynOpt";
    case MHGameEventID::MainInstanceEnter:
        return "MHGameEventID::MainInstanceEnter";
    case MHGameEventID::FadingInStartNotify:
        return "MHGameEventID::FadingInStartNotify";
    case MHGameEventID::InstanceClearingStart:
        return "MHGameEventID::InstanceClearingStart";
    case MHGameEventID::InstanceClearingFinish:
        return "MHGameEventID::InstanceClearingFinish";
    case MHGameEventID::WebPageDownloadNotify:
        return "MHGameEventID::WebPageDownloadNotify";
    case MHGameEventID::KeyMapInputNotify:
        return "MHGameEventID::KeyMapInputNotify";
    case MHGameEventID::NewCreateRoleRsp:
        return "MHGameEventID::NewCreateRoleRsp";
    case MHGameEventID::NewRoleListRsp:
        return "MHGameEventID::NewRoleListRsp";
    case MHGameEventID::NewDeleteRoleRsp:
        return "MHGameEventID::NewDeleteRoleRsp";
    case MHGameEventID::NewSelecteRoleRsp:
        return "MHGameEventID::NewSelecteRoleRsp";
    case MHGameEventID::FangchengmiTimeNotify:
        return "MHGameEventID::FangchengmiTimeNotify";
    case MHGameEventID::ChangeBullet:
        return "MHGameEventID::ChangeBullet";
    case MHGameEventID::GameStageChanged:
        return "MHGameEventID::GameStageChanged";
    case MHGameEventID::ChangeCharacterAvatar:
        return "MHGameEventID::ChangeCharacterAvatar";
    case MHGameEventID::SelectCharacter:
        return "MHGameEventID::SelectCharacter";
    case MHGameEventID::DeleteRole:
        return "MHGameEventID::DeleteRole";
    case MHGameEventID::CreateRole:
        return "MHGameEventID::CreateRole";
    case MHGameEventID::ResponeLineInfoRefresh:
        return "MHGameEventID::ResponeLineInfoRefresh";
    case MHGameEventID::RoleShow:
        return "MHGameEventID::RoleShow";
    case MHGameEventID::GuideStepStart:
        return "MHGameEventID::GuideStepStart";
    case MHGameEventID::GuideStepStop:
        return "MHGameEventID::GuideStepStop";
    case MHGameEventID::UIGuideStepComplete:
        return "MHGameEventID::UIGuideStepComplete";
    case MHGameEventID::ShowOrHideDialogIcon:
        return "MHGameEventID::ShowOrHideDialogIcon";
    case MHGameEventID::SplashMovieEsc:
        return "MHGameEventID::SplashMovieEsc";
    case MHGameEventID::RequestSelectLine:
        return "MHGameEventID::RequestSelectLine";
    case MHGameEventID::RequestEntrustList:
        return "MHGameEventID::RequestEntrustList";
    case MHGameEventID::AcceptEntrustTask:
        return "MHGameEventID::AcceptEntrustTask";
    case MHGameEventID::GiveUpEntrustTask:
        return "MHGameEventID::GiveUpEntrustTask";
    case MHGameEventID::EntrustSelHuntBox:
        return "MHGameEventID::EntrustSelHuntBox";
    case MHGameEventID::RequestAllEntrustList:
        return "MHGameEventID::RequestAllEntrustList";
    case MHGameEventID::MinimapRegionState:
        return "MHGameEventID::MinimapRegionState";
    case MHGameEventID::MinimapLinkState:
        return "MHGameEventID::MinimapLinkState";
    case MHGameEventID::MinimapPointState:
        return "MHGameEventID::MinimapPointState";
    case MHGameEventID::RequestPaidBox:
        return "MHGameEventID::RequestPaidBox";
    case MHGameEventID::TutorialStart:
        return "MHGameEventID::TutorialStart";
    case MHGameEventID::TutorialActionExecuted:
        return "MHGameEventID::TutorialActionExecuted";
    case MHGameEventID::UIWidgetShow:
        return "MHGameEventID::UIWidgetShow";
    case MHGameEventID::UIWidgetHide:
        return "MHGameEventID::UIWidgetHide";
    case MHGameEventID::AirShipState:
        return "MHGameEventID::AirShipState";
    case MHGameEventID::PVPSelHuntBox:
        return "MHGameEventID::PVPSelHuntBox";
    case MHGameEventID::PetShopAvatarView:
        return "MHGameEventID::PetShopAvatarView";
    case MHGameEventID::TrackViewEvent:
        return "MHGameEventID::TrackViewEvent";
    case MHGameEventID::ChangeCharacterAvatarWithGreatFace:
        return "MHGameEventID::ChangeCharacterAvatarWithGreatFace";
    case MHGameEventID::PvpOpenReadyPlayerInfoUI:
        return "MHGameEventID::PvpOpenReadyPlayerInfoUI";
    case MHGameEventID::TempPvpLineupReq:
        return "MHGameEventID::TempPvpLineupReq";
    case MHGameEventID::TempPvpQuitLineReq:
        return "MHGameEventID::TempPvpQuitLineReq";
    case MHGameEventID::WeaponTalentSkillActivated:
        return "MHGameEventID::WeaponTalentSkillActivated";
    case MHGameEventID::WeaponTalentSkillUnactivated:
        return "MHGameEventID::WeaponTalentSkillUnactivated";
    case MHGameEventID::WeaponTalentSkillUpgraded:
        return "MHGameEventID::WeaponTalentSkillUpgraded";
    case MHGameEventID::WeaponTalentSkillSlotUnlock:
        return "MHGameEventID::WeaponTalentSkillSlotUnlock";
    case MHGameEventID::FarmAdvGatherCutSceneEnd:
        return "MHGameEventID::FarmAdvGatherCutSceneEnd";
    case MHGameEventID::OpenWeaponTutorialVideo:
        return "MHGameEventID::OpenWeaponTutorialVideo";
    case MHGameEventID::CloseWeaponTutorialVideo:
        return "MHGameEventID::CloseWeaponTutorialVideo";
    case MHGameEventID::OpenWeaponTutorialClear:
        return "MHGameEventID::OpenWeaponTutorialClear";
    case MHGameEventID::CloseWeaponTutorialClear:
        return "MHGameEventID::CloseWeaponTutorialClear";
    case MHGameEventID::OpenCommonFunctionalList:
        return "MHGameEventID::OpenCommonFunctionalList";
    case MHGameEventID::CloseCommonFunctionalList:
        return "MHGameEventID::CloseCommonFunctionalList";
    case MHGameEventID::StartCommonFunctionByID:
        return "MHGameEventID::StartCommonFunctionByID";
    case MHGameEventID::NotifyFGTutorialVideoClosed:
        return "MHGameEventID::NotifyFGTutorialVideoClosed";
    case MHGameEventID::RequestOpenEquipShow:
        return "MHGameEventID::RequestOpenEquipShow";
    case MHGameEventID::ShowEquipShowItemByEvent:
        return "MHGameEventID::ShowEquipShowItemByEvent";
    case MHGameEventID::FGNotifyUIShow_HideMouse:
        return "MHGameEventID::FGNotifyUIShow_HideMouse";
    case MHGameEventID::NotifyFGWidgetUnloaded:
        return "MHGameEventID::NotifyFGWidgetUnloaded";
    case MHGameEventID::ActivityRequestSimpleData:
        return "MHGameEventID::ActivityRequestSimpleData";
    case MHGameEventID::ActivityRequestDetailInfo:
        return "MHGameEventID::ActivityRequestDetailInfo";
    case MHGameEventID::MarqueenAddressUpdate:
        return "MHGameEventID::MarqueenAddressUpdate";
    case MHGameEventID::ActivityRequestUniqueID:
        return "MHGameEventID::ActivityRequestUniqueID";
    case MHGameEventID::OnResponseActivityResponseSimpleData:
        return "MHGameEventID::OnResponseActivityResponseSimpleData";
    case MHGameEventID::OnResponseActivityResponseDetailInfo:
        return "MHGameEventID::OnResponseActivityResponseDetailInfo";
    case MHGameEventID::OnResponseActivityResponseAcceptTask:
        return "MHGameEventID::OnResponseActivityResponseAcceptTask";
    case MHGameEventID::OnResponseActivityResponsePickupItem:
        return "MHGameEventID::OnResponseActivityResponsePickupItem";
    case MHGameEventID::OnRoundSwfDownloadFinished:
        return "MHGameEventID::OnRoundSwfDownloadFinished";
    case MHGameEventID::OnChangeTownInstance:
        return "MHGameEventID::OnChangeTownInstance";
    case MHGameEventID::OperationActivitySimpleDataReady:
        return "MHGameEventID::OperationActivitySimpleDataReady";
    case MHGameEventID::OperationActivityDataExecRsp:
        return "MHGameEventID::OperationActivityDataExecRsp";
    case MHGameEventID::OpenChargeInternalUrl:
        return "MHGameEventID::OpenChargeInternalUrl";
    case MHGameEventID::S2CUpdateOperationActData:
        return "MHGameEventID::S2CUpdateOperationActData";
    case MHGameEventID::S2CUpdateOperationItemCountsRefresh:
        return "MHGameEventID::S2CUpdateOperationItemCountsRefresh";
    case MHGameEventID::S2CUpdateOperationItemCountsRefreshData:
        return "MHGameEventID::S2CUpdateOperationItemCountsRefreshData";
    case MHGameEventID::WeaponUnlockViewEnableSkillVideo:
        return "MHGameEventID::WeaponUnlockViewEnableSkillVideo";
    case MHGameEventID::WeaponUnlockPrework:
        return "MHGameEventID::WeaponUnlockPrework";
    case MHGameEventID::OperationActivityHasNewData:
        return "MHGameEventID::OperationActivityHasNewData";
    case MHGameEventID::ExitWindowDelay:
        return "MHGameEventID::ExitWindowDelay";
    case MHGameEventID::InstanceSelectQuickMatch:
        return "MHGameEventID::InstanceSelectQuickMatch";
    case MHGameEventID::InstanceSelectHubEnterLevel:
        return "MHGameEventID::InstanceSelectHubEnterLevel";
    case MHGameEventID::InstanceCaptainSelectLevel:
        return "MHGameEventID::InstanceCaptainSelectLevel";
    case MHGameEventID::InstanceSelect_UnlockEliteLevelID:
        return "MHGameEventID::InstanceSelect_UnlockEliteLevelID";
    case MHGameEventID::LevelUnlockNotify:
        return "MHGameEventID::LevelUnlockNotify";
    case MHGameEventID::SelectTutorialWeapon:
        return "MHGameEventID::SelectTutorialWeapon";
    case MHGameEventID::WeaponTutorialStepUpdated:
        return "MHGameEventID::WeaponTutorialStepUpdated";
    case MHGameEventID::WeaponTutorialClearAllShortcut:
        return "MHGameEventID::WeaponTutorialClearAllShortcut";
    case MHGameEventID::UpdateShortcutItem:
        return "MHGameEventID::UpdateShortcutItem";
    case MHGameEventID::WeaponTutorialUpdateUIWeaponType:
        return "MHGameEventID::WeaponTutorialUpdateUIWeaponType";
    case MHGameEventID::SetTeachUIClickMode:
        return "MHGameEventID::SetTeachUIClickMode";
    case MHGameEventID::TeachUIClickedEvent:
        return "MHGameEventID::TeachUIClickedEvent";
    case MHGameEventID::LobbyChanged:
        return "MHGameEventID::LobbyChanged";
    case MHGameEventID::VipLevelNotify:
        return "MHGameEventID::VipLevelNotify";
    case MHGameEventID::UIWidgetMinimize:
        return "MHGameEventID::UIWidgetMinimize";
    case MHGameEventID::UIWidgetMaximize:
        return "MHGameEventID::UIWidgetMaximize";
    case MHGameEventID::BeginChangeFace:
        return "MHGameEventID::BeginChangeFace";
    case MHGameEventID::EndChangeFace:
        return "MHGameEventID::EndChangeFace";
    case MHGameEventID::ChangeFaceResponse:
        return "MHGameEventID::ChangeFaceResponse";
    case MHGameEventID::PlayerBeautyAcountInfoNotify:
        return "MHGameEventID::PlayerBeautyAcountInfoNotify";
    case MHGameEventID::MvMPetMonsterDeadNotify:
        return "MHGameEventID::MvMPetMonsterDeadNotify";
    case MHGameEventID::ReturnTownNotify:
        return "MHGameEventID::ReturnTownNotify";
    case MHGameEventID::ReturnChooseRoleNotify:
        return "MHGameEventID::ReturnChooseRoleNotify";
    case MHGameEventID::SaveNetworkSequenceNotify:
        return "MHGameEventID::SaveNetworkSequenceNotify";
    case MHGameEventID::OnLeaveCamp:
        return "MHGameEventID::OnLeaveCamp";
    case MHGameEventID::OnLeaveJumpArea:
        return "MHGameEventID::OnLeaveJumpArea";
    case MHGameEventID::OnLeaveFarm:
        return "MHGameEventID::OnLeaveFarm";
    case MHGameEventID::OnEnterFarm:
        return "MHGameEventID::OnEnterFarm";
    case MHGameEventID::OnEnterCamp:
        return "MHGameEventID::OnEnterCamp";
    default:
        return "Unknown MHGameEventID!";
    }
}

enum class MHLogicEventID
{
   ServerConnectStart = 0x1,
   ServerConnectSucceeded = 0x2,
   ServerConnectFailed = 0x3,
   PingResult = 0x4,
   Disconnected = 0x5,
   EnterInstanceResponse = 0x6,
   InstanceVerifyResponse = 0x7,
   GlobalErrcode = 0x8,
   CheckVersionResponse = 0x9,
   RoleListResponse = 0xA,
   SelectRoleResponse = 0xB,
   TeamAddRoleResponse = 0xC,
   TeamUpdataRoleResponse = 0xD,
   PlayerTaskAcceptResponse = 0xE,
   PlayerTaskGiveUpResponse = 0xF,
   PVPScoreResponse = 0x10,
   PVPEventResponse = 0x11,
   PlayerTaskUIResetRespone = 0x12,
   TeamInviteAsk = 0x13,
   TeamInviteRsp = 0x14,
   TeamJoinRsp = 0x15,
   NpcTaskUIResetRespone = 0x16,
   TeamLeaderNtf = 0x17,
   TaskComplete = 0x18,
   UpdateInventoryItemNtf = 0x19,
   UpdateInventoryInfoNtf = 0x1A,
   UpdateAvatarInfo = 0x1B,
   ShowInfo = 0x1C,
   ChatMessage = 0x1D,
   UIWidgets = 0x1E,
   UIStageMode = 0x1F,
   UpdateNpcTalkContentNtf = 0x20,
   NpcTalkTitleListUpdate = 0x21,
   PlayerTaskUpdte = 0x23,
   PlayerTaskTargetUpdte = 0x24,
   PlayerFixedAwardItemUpdte = 0x25,
   PlayerSelectedAwardItemUpdte = 0x26,
   PlayerOtherInfoUpdte = 0x27,
   PlayerTaskTrackUpdte = 0x28,
   NpcTaskTargetUpdte = 0x29,
   NpcFixedAwardItemUpdte = 0x2A,
   IME_Ack = 0x2B,
   NpcOtherInfoUpdte = 0x2C,
   MinimapSelfPlayerUpdate = 0x2D,
   MinimapOtherPlayerUpdate = 0x2E,
   MinimapItemUpdate = 0x2F,
   MiniMapNpcUpdate = 0x30,
   MiniMapInfoUpdate = 0x31,
   UpdateActionSlotID = 0x32,
   UIInputAction = 0x33,
   ChatLog_Enter = 0x34,
   ChatLabelUpdata = 0x35,
   UpdateTeamInfo = 0x36,
   UpdateTeamMemberInfo = 0x37,
   DelTeamMemberInfo = 0x38,
   UpdateSelfGreenHp = 0x39,
   UpdateSelfRedHp = 0x3A,
   UpdateForgeItem = 0x3B,
   UpdateForgeMat = 0x3C,
   UpdateItemCostAndIsForge = 0x3D,
   UpdateMainHudData = 0x3E,
   ShowMessgaeBox = 0x3F,
   exitTeam = 0x40,
   UpdateSelfStamina = 0x41,
   ChangeTeamLeader = 0x42,
   PlayerBaseInfo = 0x43,
   PlayerNameInfo = 0x44,
   InstanceMainInfo = 0x45,
   InstanceModeList = 0x46,
   SubInstanceInfo = 0x47,
   SubInstanceMonsterInfo = 0x48,
   StartCountDown = 0x49,
   FailEnterInfo = 0x4A,
   InstanceComplete = 0x4B,
   EnterInstanceInterrupt = 0x4C,
   Instance_Evaluate = 0x4D,
   Instance_SettleAccount = 0x4E,
   Instance_InstanceOperation = 0x4F,
   Instance_Next = 0x50,
   Instance_TeamPlayerInfo = 0x51,
   ShowDamageFloat = 0x52,
   UpdateBulletNum = 0x53,
   BulletReloadEnd = 0x54,
   SupplyItemUpate = 0x55,
   PushPromptMessage = 0x56,
   UpdateBladeSharpness = 0x57,
   UpdateRageValue = 0x58,
   InitForgeViewData = 0x59,
   UpdateTachiEnergy = 0x5A,
   PlayAnimation = 0x5B,
   UpdatePlayerState = 0x5C,
   ShowRightMenu = 0x5D,
   ShowMainMenu = 0x5E,
   QuestAddSucceed = 0x5F,
   CraftSuccess = 0x60,
   ToolTipsGetData = 0x61,
   InitSystemConfig = 0x62,
   GetStaticItemData = 0x63,
   SupplyBoxInit = 0x64,
   SupplyBoxGetRsp = 0x65,
   ShowOrHideLock = 0x66,
   UpdateLockState = 0x67,
   ShowOrHideTargetLock = 0x68,
   ChangeLoginBtnState = 0x6A,
   GetLoginLastAccount = 0x6B,
   ResponseNpcSaleInfo = 0x6C,
   ResponseItemIconItemInfo = 0x6D,
   ItemOperateRsp = 0x6E,
   ItemInit = 0x6F,
   OnLocalLogicPlayerEnter = 0x70,
   OnShoppingOperationResult = 0x71,
   NpcTalkTitleList_OtherInfo = 0x72,
   UpdateManufactureListInfo = 0x73,
   ManufactureDetailedInfo = 0x74,
   UpdateManufactureProficiency = 0x75,
   UpdateManufactureStudyListInfo = 0x76,
   ManufactureMaterialInfo = 0x77,
   UpdateAvailableSlot = 0x78,
   CreateDialogFromCPP = 0x79,
   GetNpcSaleInfoBySaleID = 0x7A,
   ShowIntoneBar = 0x7B,
   UpdateExpandSlotCost = 0x7C,
   TradeStart = 0x7D,
   TradeupdataInfo = 0x7E,
   TradeClose = 0x7F,
   TradeLock = 0x80,
   TradeComplete = 0x81,
   TradeItemHandler = 0x82,
   TradeupdataItem = 0x83,
   ActionPointDataInit = 0x84,
   ActionPointDataUpdate = 0x85,
   ActionPointSelected = 0x86,
   UpdateCharExp = 0x87,
   UIInfo = 0x88,
   OtherTeamListInfo = 0x89,
   OtherTeamMemberInfo = 0x8A,
   LevelGroupInfo = 0x8B,
   InstanceClearingReadyCountDownStart = 0x8C,
   InstanceClearingReadyCountDownCompleted = 0x8D,
   EquipRankLevelUPResult = 0x8E,
   EquipRankLevelup2Six = 0x8F,
   PlayerInteractBegin = 0x90,
   InstanceFinishMsg = 0x91,
   InstancePlayerScore = 0x92,
   InstanceRecvResult = 0x93,
   InstancePlayerExpReward = 0x94,
   InstancePlayerGoldReward = 0x95,
   AcceptTask = 0x96,
   GiveupTask = 0x97,
   SubmitTask = 0x98,
   ShareTask = 0x99,
   TaskErr = 0x9A,
   TaskContentExec = 0x9B,
   TaskExec = 0x9C,
   TaskContentStateChange = 0x9D,
   TaskStateChange = 0x9E,
   TaskLoad = 0x9F,
   SyncDaily = 0xA0,
   RefreshDaily = 0x3E8,
   RefreshSchedule = 0x3E9,
   RefreshxDaily = 0x3EA,
   InstanceInitEntryInfo = 0xA1,
   PlayerInteractEnd = 0xA2,
   TipsEquipAdditionalInfo = 0xA3,
   UpdateFriendList = 0xA4,
   UpdateFriendDetailInfo = 0xA5,
   UpdateManuSkillInfo = 0xA6,
   ChangeTownServer = 0xA7,
   EquipEnforceBoard = 0xA8,
   UpdateLevelScoreAvatar = 0xA9,
   SetInstanceBoxData = 0xAA,
   ActionBoardDownloadComplete = 0xAB,
   PerfectReloadInput = 0xAC,
   BeginAmmoReload = 0xAD,
   KeyMapInputNotifyUI = 0xAE,
   InstanceUpdateRewardScore = 0xAF,
   ResetBulletTab = 0xB0,
   UpdateInstanceAssess = 0xB1,
   AvailableAmmoTypeChange = 0xB2,
   AmmoSelectUIHide = 0xB3,
   AddBuffer = 0xB4,
   DeleteBuffer = 0xB5,
   MouseMode = 0xB6,
   UpdateAvatarImageMainUI = 0xB7,
   HideMessageBox = 0xB8,
   EnablePlayerTaskDlgMouseClick = 0xB9,
   UpdateCatCarNumber = 0xBA,
   UpdateAvatarToMainIcon = 0xBB,
   TradeAddItemError = 0xBC,
   EnableTidyBtn = 0xBD,
   UpdateClockStatus = 0xBE,
   UpdateBuffer = 0xBF,
   UpdateLevelName = 0xC0,
   ShowPlayerFrameAvatarFrame = 0xC1,
   ManufactureSucceed = 0xC2,
   OnCtrlC = 0xC3,
   OnCtrlV = 0xC4,
   ShowWaitQueue = 0xC5,
   ItemMoveSwapUpdate = 0xC6,
   GameChatMessage = 0xC7,
   OnRequireExitGame = 0xC8,
   PackItemList = 0xC9,
   InstanceFinishNotify = 0xCA,
   RefreshDailyStatus = 0xCB,
   ObtainScheduleRewardList = 0xCC,
   ObtainScheduleTargetList = 0xCD,
   ScheduleError = 0xCE,
   ScheduleFetchPrize = 0xCF,
   UpdateMailList = 0xD0,
   RefreshMailDetail = 0xD1,
   SendMailResponse = 0xD2,
   DeleteMailResponse = 0xD3,
   RequireMailList = 0xD4,
   FarmSwich = 0xD5,
   FarmOwner = 0xD6,
   UpdateBowEnergy = 0xD7,
   ReturnMailResponse = 0xD8,
   ShowOrHideVehicleSight = 0xD9,
   FarmUpgradeResponse = 0xDA,
   ShowFarmNoticeHud = 0xDB,
   GuildErr = 0xDC,
   GuildNotify = 0xDD,
   GetGuilds = 0xDE,
   FindGuilds = 0xFA,
   GetGuildApplys = 0xDF,
   GetGuildInvitations = 0xE0,
   GetGuildDetail = 0xE1,
   GetGuildTitles = 0xE2,
   GetLeader = 0xE3,
   GetSelf = 0xE4,
   GetGuilderDetail = 0xE5,
   ClearGuilderDetail = 0xE6,
   GetGuilder = 0xE7,
   GetGuildersCount = 0xE8,
   GetGuilders = 0xE9,
   GuildDepotFetchCountUpdate = 0xEA,
   GuildRandCommoditiesUpdate = 0xEB,
   GuildDepotOpCountUpdate = 0xEC,
   GetGuildQuest = 0xED,
   UpdateDualBladesGhost = 0xF9,
   HunterStarAddCards = 0xFB,
   PetItemTipsGetData = 0xFC,
   ShowHunterStarUIByCollection = 0xFD,
   UpdateNewBattleRemainTime = 0xFE,
   UpdateFarmInfo = 0xFF,
   UpdatePetList = 0x100,
   NotifyPetNameChange = 0x101,
   NotifyPetDetailInfo = 0x102,
   NotifyPetSkillLearn = 0x103,
   NotifyPetEquipOnOff = 0x104,
   NotifyPetSkillEquipOnOff = 0x105,
   UpdateLevelTrackData = 0x106,
   UpdateEntrustTrackState = 0x107,
   FarmFacilityLevelUp = 0x108,
   EntrustListUpdate = 0x109,
   NotifyPetAvatarUpdate = 0x10A,
   NotifyForgeTargetItem = 0x10B,
   TalkErr = 0x10C,
   EntrustGoChangeMemState = 0x10D,
   EntrustGoMemSelPackage = 0x10E,
   CatCuisineMakeSucceed = 0x10F,
   FSM2UI = 0x110,
   UpdateGiftPackData = 0x111,
   LIneInfoNotify = 0x112,
   AllEntrustListUpdate = 0x113,
   ManufactureAddSkillDegree = 0x114,
   InstanceMonsterNewSize = 0x115,
   UpdatePlayerProperty = 0x116,
   LocalPlayerDeadNotify = 0x117,
   MailSystemError = 0x118,
   ChangeLevelNotifyUI = 0x119,
   PlaySoundSfx = 0x11A,
   NewLineFullNotify = 0x11B,
   ChatEnterNotifyUI = 0x11C,
   SchedulePrizeRefresh = 0x11D,
   OnEntrustAcceptOk = 0x11E,
   OnPvpAssignmentComplete = 0x11F,
   OnPvpSpecialAward = 0x120,
   OnPvpScore = 0x121,
   UpdatePaidBoxItem = 0x122,
   TeamPushVecNtf = 0x123,
   CreateRelayConn = 0x124,
   NpcAtdGiveGift = 0x125,
   NpcAtdErr = 0x126,
   NpcAtdAddValue = 0x127,
   ShowOrHideCommonProgress = 0x128,
   UpdateCommonProgressValue = 0x129,
   PvpPlayerSetTrap = 0x12A,
   PvpPlayerKillPlayer = 0x12B,
   PvpPlayerGetGodLuck = 0x12C,
   PvpGameOver = 0x12D,
   PvpScoreRecording = 0x12E,
   HunterStarFinishCard = 0x12F,
   UpdateMailUnReadNum = 0x130,
   RequestNpcAtdInfoUpdate = 0x131,
   RankListUpdate = 0x132,
   WeaponTalentLockStatusUpdate = 0x133,
   WeaponTalentEquipTalentUpdate = 0x134,
   UpdateHammerStatus = 0x135,
   ThousandLayerSupplyMemSel = 0x136,
   CDGroupStatus = 0x137,
   RankErr = 0x138,
   RankRewardFetched = 0x139,
   WeaponTalentEquipProfoundSkill = 0x13A,
   NpcAtdLevelUp = 0x13B,
   NpcOrgLevelUp = 0x13C,
   NpcAtdPreferUnlock = 0x13D,
   WeaponTalentError = 0x13E,
   HunterStarRecordCardLevelUp = 0x13F,
   UpdateGiantSwordStatus = 0x140,
   UpdatePetBattleInfo = 0x141,
   LocalPlayerEnterGame = 0x142,
   LocalPlayerSelectPlayAnimation = 0x143,
   EnableCreateRoleUI = 0x144,
   RefreshTitleList = 0x145,
   RefreshTitleProperList = 0x146,
   HunterStarBackSpaceEvent = 0x147,
   DelAllBuffs = 0x148,
   GuildNotifyToUI = 0x149,
   SystemUnLockNotify = 0x14A,
   PlayerTaskUIShowNotify = 0x14B,
   OpenReviveUI = 0x14C,
   OnTradeBeginAsk = 0x14D,
   FriendOnlineRemind = 0x14E,
   TeamInviteAndRequestMessagbox = 0x14F,
   BowBottleListUpdate = 0x150,
   UpdateCatTicketNum = 0x151,
   UpdateReviveCoinNum = 0x152,
   PlayerLevelChange = 0x153,
   DeathCameraPlayOver = 0x154,
   LocalPlayerAfterEnterBattleGround = 0x155,
   CentreScreenInfoShow = 0x156,
   PetPropertyUpdateNotify = 0x157,
   UpdateGuildCampInfo = 0x158,
   GuildCampLevelUp = 0x159,
   SupplyBoxType = 0x15A,
   BattlePvpRemainTime = 0x15B,
   PlayerRevive = 0x15C,
   requestOpenPvpScoreList = 0x15D,
   localPlayerScoreAdded = 0x15E,
   PvpHunterBoxOpen = 0x15F,
   PvpHunterBoxUpdate = 0x160,
   PvpHunterBoxClose = 0x161,
   PvpGroupSettlementUIOpen = 0x162,
   NewSkillNotifyToSkillVideoUi = 0x163,
   MainHudBattleStateNotify = 0x164,
   TalkExec = 0x165,
   FarmPetTraining = 0x166,
   SensitiveVerify = 0x167,
   SafeModeOpenUrlNotify = 0x168,
   LimitUrlVerify = 0x169,
   GetManageUrl = 0x16A,
   MapTypeNotify = 0x16B,
   DeadAnimPlayOver = 0x16C,
   PvpScoreListRequirement = 0x16D,
   LocalPlayerEnterPvp = 0x16E,
   SameFactionPlayerEnterPvp = 0x16F,
   PvpPlayerStatusChanged = 0x170,
   PvpPlayerBufferChanged = 0x171,
   PvpPlayerConnectionChanged = 0x172,
   PvpHunterBoxJoin = 0x173,
   ExternalWndCountUpdate = 0x174,
   ChangeQTTalkState = 0x175,
   SkillVideoUINotify = 0x176,
   BattleGroundChanged = 0x177,
   InventoryChangedNotify = 0x178,
   PVPStrikeKillingEvent = 0x179,
   BannerUpdate = 0x17A,
   PvpPreparePlayerJoin = 0x17B,
   PvpPreparePlayerLeave = 0x17C,
   PvpPrepareStageStart = 0x17D,
   PvpPrepareStageFinish = 0x17E,
   PvpPlayerGetScore = 0x17F,
   PvpBattleRealStart = 0x180,
   PvpBattleRealStartWithoutReadyGo = 0x181,
   GetGuildLearnedSkills = 0x182,
   GetGuildCandidateSkills = 0x183,
   PvpBattleShowFlashScoreBg = 0x184,
   Pvp1stScoreStage = 0x185,
   VehicleAmmoExhaustChangeBullet = 0x186,
   DismissClanNotify = 0x187,
   QuitClanNotify = 0x188,
   KickClanNotify = 0x189,
   OnGetClanersInfoNotify = 0x18A,
   OnGetClanDetailNotify = 0x18B,
   OnCreateClaneError = 0x18C,
   MartGoodsListRsp = 0x18D,
   ClanInviteRequire = 0x18E,
   EnterCreateRole = 0x18F,
   LeaveCreateRole = 0x190,
   VehicleAmmoUpdate = 0x191,
   OnGuildLeagueBeginAsk = 0x192,
   OnCreateClanOK = 0x193,
   OnGuildLeagueSignUpAgree = 0x194,
   GuildDepotInit = 0x195,
   GuildDepotNameUpdate = 0x196,
   GuildItemOperate = 0x197,
   GuildDepotSortBtnEnable = 0x198,
   NetNotifyClanersRefresh = 0x199,
   PvpSameTeamGetScore = 0x19A,
   UpdateGuildInventoryItemNtf = 0x19B,
   GetOtherPlayerInfoNotify = 0x19C,
   MainInstanceEnterType = 0x19D,
   PetTalentSkillRrefresh = 0x19E,
   EquipUpgradeResult = 0x19F,
   IncDepotColumnCount = 0x1A0,
   MartGoodBuyRsp = 0x1A1,
   TournamentGetRanksNotify = 0x1A2,
   TournamentSearchPlayerNotify = 0x1A3,
   UpdateTournamentRankList = 0x1A4,
   SpoorErr = 0x1A5,
   SpoorFinish = 0x1A6,
   SpoorPrizeFetched = 0x1A7,
   SpoorNewPrize = 0x1A8,
   AddSpoorPoints = 0x1A9,
   WarningShow = 0x1AA,
   NewTaskAutoAccept = 0x1AB,
   SensitiveVerifyWithTips = 0x1AC,
   BuffItemNtf = 0x1AD,
   ItemDecompose = 0x1AE,
   AuctionEventToUI = 0x1AF,
   ShortCutBarFlashByWidgetID = 0x1B0,
   CommonGroupLocalPlayerEnter = 0x1B1,
   CommonGroupPlayerStatusChanged = 0x1B2,
   CommonGroupPlayerBufferChanged = 0x1B3,
   CommonGroupConnectionChanged = 0x1B4,
   CommonGroupOtherPlayerEnter = 0x1B5,
   RefreshScultUI = 0x1B6,
   AirshipControlState = 0x1B7,
   RefreshTreasureList = 0x1B8,
   RefreshTreasureAwardList = 0x1B9,
   FarmAdvGatherRsp = 0x1BA,
   OnAirshipControlStateSelect = 0x1BB,
   CallFGPlayPetFireCutscene = 0x1BC,
   OpenPetFireConfirm = 0x1BD,
   UpdateEntrustMoney = 0x1BE,
   RefreshCreateRoleUIFromLocalData = 0x1BF,
   UpdateSpecLevelInfo = 0x1C0,
   HornMessage = 0x1C1,
   BattWatchModeNotify = 0x1C2,
   GuildMatchEventToUI = 0x1C3,
   SwitchWindowMode = 0x1C4,
   SystemNoticMsg = 0x1C5,
   RapidHuntHisListUpdate = 0x1C6,
   CallFGPlayEquipForgeCutscene = 0x1C7,
   EquipForgeCutsceneFinishedNotify = 0x1C8,
   OpenVIPMsg = 0x1C9,
   OpenVIPGrowthMsg = 0x1CA,
   OpenVIPProfitMsg = 0x1CB,
   VIPServiceExpireMsg = 0x1CC,
   FeatureUpdated = 0x1CD,
   ExchangeEventToUI = 0x1CE,
   EnableOITTest = 0x1CF,
   GuildMatchErr = 0x1D0,
   OpenMartVipPanelNotify = 0x1D1,
   ActivityAddAward = 0x1D2,
   ActivityFetchAward = 0x1D3,
   ActivityRefreshAward = 0x1D4,
   ActivityErr = 0x1D5,
   HunterGroupPvpScoreNotify = 0x1D6,
   UpdateQuickMatchInfo = 0x1D7,
   ActivityAwardRefresh = 0x1D8,
   UIResolutionChangedNotify = 0x1D9,
   LocalPlayerWeaponUnlock = 0x1DA,
   SwitchNVFur = 0x1DB,
   OnAutoReviveEvent = 0x1DC,
   UpdatePetMonsterGreenHp = 0x1DD,
   UpdatePetMonsterRedHp = 0x1DE,
   UpdatePetMonsterStamina = 0x1DF,
   UpdatePetMonsterRageValue = 0x1E0,
   UpdatePetMonsterState = 0x1E1,
   UpdatePetMonsterMainInfo = 0x1E2,
   OpenMonsterHud = 0x1E3,
   MonsterIndexNotify = 0x1E4,
   PetMonsterUIInfoNotify = 0x1E5,
   PetMonsterSkillInfoUpdate = 0x1E6,
   PetMonsterSkillUse = 0x1E7,
   UpdateCursorVisible = 0x1E8,
   GetAFluteNote = 0x1E9,
   GetMelodyEffect = 0x1EA,
   LostMelodyEffect = 0x1EB,
   RemoveMelody = 0x1EC,
   FluteOpernKeyClick = 0x1ED,
   FluteChooseOpern = 0x1EE,
   UpgradeFluteNote = 0x1EF,
   GenerateMelody = 0x1F0,
   InvokeFluteTune = 0x1F1,
   WakePointNotfiy = 0x1F3,
   WakeStatusNotify = 0x1F4,
   WakeTimesResetNotify = 0x1F5,
   ShowLobbyUI = 0x1F6,
   RefreshMartCredit = 0x1F7,
   LookOtherTeamInfoRefresh = 0x1F8,
   UpdateReviveUI = 0x1F9,
   LineUpRetry = 0x1FA,
   GameCountDown = 0x1FB,
   ItemExchangeResult = 0x1FC,
   Enable3DMode = 0x1FD,
   Ipvp_TeamScoreUpdate = 0x1FE,
   Ipvp_PlayerScoreUpdate = 0x1FF,
   WeaponTraceInfoUpdate = 0x200,
   InstanceSelect_responseServerUnlockLevel = 0x201,
   WeaponForgeSuccess = 0x202,
   CountDownNumberShow = 0x203,
   Ipvp_Settlement = 0x204,
   UnlockPetSkillSlot = 0x205,
   EquipTransferResult = 0x206,
   FarmCDReduceResult = 0x207,
   TransferResult = 0x208,
   UpdateTutorialQTE = 0x209,
   UpdateItemData = 0x20A,
   OperationActivity_FileDownloaded = 0x20B,
   CallFGPlayPetEmployCutscene = 0x20C,
   WeaponTutorialWeaponPreview = 0x20D,
   UpdateScriptActivity = 0x20E,
   RandomShopRefresh = 0x20F,
   UpdateQueueInfo = 0x210,
   TaskSubmitting = 0x211,
   DragonBoxOpenEnd = 0x212,
   DragonBoxDetail = 0x213,
   DragonBoxPrizeFetch = 0x214,
   DragonBoxArgs = 0x215,
   DragonBoxOpen = 0x216,
   PetTreasureDigResut = 0x217,
   ChangeUIActionMode = 0x218,
   TryPurchaseVip = 0x219,
   WinXinPayUrl = 0x21A,
   VipUpdate = 0x21B,
   OpenMartView = 0x21C,
   RefreshTraceUINotity = 0x21D,
   PlayerResetPositionStateChange = 0x21E,
   PvpBattleListUpdate = 0x21F,
   GuildSelectQualifierInstance = 0x220,
   BattleSupplyCatUnLock = 0x221,
   GunLanceUpdate = 0x222,
   ContestRankTitleUpdate = 0x223,
   ContestRankTeamMatesListUpdate = 0x224,
   ShowOrHideRadarProgress = 0x225,
   UpdateRadarProgressValue = 0x226,
   ReportPlayerResult = 0x227,
   NewItemNotice = 0x228,
   MartRefresh = 0x229,
   QuickOpenVIPMsg = 0x22A,
   IncomingNoticMsg = 0x22B,
   TaskTraceInfoUpdate = 0x22C,
   CancelWeiXinOrderOk = 0x22D,
   WeiXinOrderTimeout = 0x22E,
   UpdateFluteOphern = 0x22F,
   FluteStateReset = 0x230,
   LocalGameSettingUpdate = 0x231,
   NetworkStatus = 0x232,
   FileCheckTimeOutReq = 0x233,
   NotifyUIBake = 0x234,
   IpvpAttackInfo = 0x235,
   IpvpMiniMapEvent = 0x236,
   YoukuReplayStatusNotify = 0x237,
   NpcTalkSendFGEvent = 0x238,
   RapidSpeak = 0x239,
   ScriptActivityLoaded = 0x23A,
   ScriptActivityLoading = 0x23B,
   GamePadSwitchToUIMode = 0x23C,
   PackItemPlayFx = 0x23D,
   StarStoneFeedOk = 0x23E,
   StarStoneRepair = 0x23F,
   UpdateEquipColorList = 0x240,
   UpdateEquipDyeView = 0x241,
   EquipDyeViewRespond = 0x242,
   StarStoneEnchaseOk = 0x243,
   RefreshInstanceRewardView = 0x244,
   WelfareMenuCanFetchStatusUpdate = 0x245,
   OperationActivityNoticeStatusUpdate = 0x246,
   FriendNotify = 0x247,
   RefreshGiveupUI = 0x248,
   NotifyGiveupUI = 0x249,
   OnReceiveNewGift = 0x24A,
   ChangeEquipNotify = 0x24B,
   LevelCatLoot = 0x24C,
   FarmEquipAvatarGetResult = 0x24D,
   Mart_FileDownloaded = 0x24E,
   UpdateEquipPlanList = 0x24F,
   PreviewEquipPlan = 0x250,
   GiveUpRevive = 0x251,
   UpdateNewCharExpBar = 0x252,
   PetMonsterSkillUseNotify = 0x253,
   CloseMonsterHud = 0x254,
   MHUIMonsterSelector = 0x255,
   ChangeWeaponInPreparation = 0x256,
   GuildNameUpdate = 0x257,
   EquipTransmogrifyOk = 0x258,
   AddTeamBuff = 0x259,
   UpdateInstanceReward = 0x25A,
   VehicleFireCDNotify = 0x25B,
   RequestQuenchEquipRsp = 0x25C,
   MatchEntryUpdateRewardState = 0x25D,
   WildHuntGuildInfo = 0x25E,
   WildHuntSignUpOk = 0x25F,
   IsFaceEditing = 0x260,
   BattleTeamNameUpdate = 0x261,
   PetShopRefresh = 0x262,
   AutoBacktoRolelist = 0x263,
   RefreshSupplyPlan = 0x264,
   SupplyPlanEquip = 0x265,
   RecvLevelCatRewardList = 0x266,
   RefreshNewEquipUpgrade = 0x267,
   GetSailMoney = 0x268,
   EnablePerfDmp = 0x269,
   SuitSkillActiveOk = 0x26A,
   SuitSkillEditOk = 0x26B,
   InstanceEnterModeUpdate = 0x26C,
   AddLikeItOk = 0x26D,
   WaitSkillListUpdate = 0x26E,
   PetStatusChangeNotify = 0x26F,
   OnDivinationStarLightened = 0x270,
   OnDivinationAnswerFeedback = 0x271,
   OnDivinationError = 0x272,
   OnDivinationGetSubject = 0x273,
   WaitSkillDeleteNotify = 0x274,
   HuntCreditCoinUsedUpdate = 0x275,
   LevelRegionChangeNotify = 0x276,
   OnQuenchSaveRsp = 0x277,
   BossFornotice = 0x278,
   RefreshThousandSupplyUI = 0x279,
   Loading_FileDownloaded = 0x27A,
   WildHuntTaskUpdate = 0x27B,
   LogParticleInfo = 0x27C,
   OnPvMScore = 0x27D,
   OneByOneChatMessage = 0x27E,
   SwitchPrivateChat = 0x27F,
   UpdateActionBarData = 0x280,
   ClearBuffIcon = 0x281,
   OnExpressionLvUp = 0x282,
   UpdateRightMenuData = 0x283,
   RefreshEquipRecast = 0x284,
   UISplashOver = 0x285,
   AddPopChat = 0x286,
   WorldMap = 0x287,
   YamatuskamiView = 0x288,
   MonolopyEvent = 0x289,
   RefreshItemRemoveBind = 0x28A,
   NewbieVideoPlay = 0x28B,
   WildHuntSecretQuestTimeUpdate = 0x28C,
   OnNewbieVideoPlayEnd = 0x28D,
   TransformSwordShapeOrValueChanged = 0x28E,
   SetTowerDefenseTemplateIdEvent = 0x28F,
   SetTowerDefenseTypeIdEvent = 0x290,
   RefreshTowerDefenseMainView = 0x291,
   RespondTowerDefenseInfo = 0x292,
   SoulStone = 0x293,
   SoulStoneStageUP = 0x294,
   SoulStoneAttrUP = 0x295,
   SoulStoneReset = 0x296,
   SoulStoneUnlock = 0x297,
   SoulStoneLevelUp = 0x298,
   EquipQuickLevelUpOk = 0x299,
   RefreshGuardStoneView = 0x29A,
   AbnormalState_Restricted_QTE_UI_Show = 0x29B,
   AbnormalState_Restricted_QTE_UI_Notify = 0x29C,
   PetMonsterPlayerDeadNotify = 0x29D,
   UpdateGuildActivityGridNotify = 0x29E,
   NewbieWeaponTutorial = 0x29F,
   OnCloseWeaponTutorialEnd = 0x2A0,
   UpdateGuildActInfoNotify = 0x2A1,
   UltimateChallengeSettlementData = 0x2A2,
   UltimateChallengeSettlement = 0x2A3,
   UltimateChallengeRankNtf = 0x2A4,
   XmasActivityRefresh = 0x2A5,
   UpdateGuildTaskDetailInfo = 0x2A6,
   UpdateSoulStonePoint = 0x2A7,
   MVPAngrySkillNotify = 0x2A8,
   RefreshGuilderActivityInfo = 0x2A9,
   PlayVideo = 0x2AA,
   UpdateProfoundData = 0x2AB,
   ReqGuildInfoData = 0x2AC,
   RewardSucced = 0x2AD,
   SetGameMaxFps = 0x2AE,
   StarHuntLevelMode = 0x2AF,
   InstanceSelect_responseServerUnlockStar = 0x2B0,
   GlidingBombUI = 0x2B1,
   GlidingBombCountsRefresh = 0x2B2,
   XmasActivityGetMeal = 0x2B3,
   ApplyForCommderResult = 0x2B4,
   RequireCommderResult = 0x2B5,
   ClearDamageNumber = 0x2B6,
   OnDivinationReset = 0x2B7,
   StartCatCuisine = 0x2B8,
   EndCatCuisine = 0x2B9,
   QuitCommanderResult = 0x2BA,
   HideUI = 0x2BB,
   OnDivinationRefresh = 0x2BC,
   RequireUltimateChangeMonsterResult = 0x2BD,
   ToggleVolFog = 0x2BE,
   OnWeaponStyleRsp = 0x2BF,
   OnSoulBeastRsp = 0x2C0,
   IdleViewWardrobeLeave = 0x2C1,
   SetFitAvatarListFinish = 0x2C2,
   OnSuperHunterUIRefresh = 0x2C3,
   OnLegendPearlUpgradeRsp = 0x2C4,
   RefreshGuideBookUI = 0x2C5,
   MaxStaAndHPChangeNotify = 0x2C6,
   MaxStaAndHPChangeNotifyLevle = 0x2C7,
   OnLegendPearlDeriveRsp = 0x2C8,
   RefreshWorldCupRankList = 0x2C9,
   RefreshHunterBattle = 0x2CA,
   GetHunterBattleShipRequireDatas = 0x2CB,
   SetHunterBattleRankDetails = 0x2CC,
   GetAllCommerce = 0x2CD,
   GetSeaSandData = 0x2CE,
   RefreshShipRequireData = 0x2CF,
   RefreshAllGuildSailData = 0x2D0,
   GetPlayerShipData = 0x2D1,
   RefreshGuildCommerceBuff = 0x2D2,
   RefreshGuildShipByGuildID = 0x2D3,
   OnEnterLevelClick = 0x2D4,
   GuildFuncRecordRsp = 0x2D5,
   RefreshWarshipTipData = 0x2D6,
   GuildInventoryRecordRsp = 0x2D7,
   RefreshGrabableTimes = 0x2D8,
   RefreshStartBoatTimes = 0x2D9,
   SecretResearchRefreshCurTabData = 0x2DA,
   SecretResearchRefreshResearchData = 0x2DB,
   IllustrateBookCollectCardRsp = 0x2DC,
   MonsterCardExchangePhyCardRsp = 0x2DD,
   MonsterCardGetRewardRsp = 0x2DE,
   MonsterCardUpdateCardInfo = 0x2DF,
   DragonBoxShopRsp = 0x2E0,
   DragonBoxFreshNumOpenRsp = 0x2E1,
   IllustrateBookGoToMonster = 0x2E2,
   DailyConsumptionOpenActivityRsp = 0x2E3,
   DailyConsumptionGetRewardRsp = 0x2E4,
};

inline const char* GetMHLogicEventIDName(MHLogicEventID event) {
    switch (event) {
    case MHLogicEventID::ServerConnectStart:
        return "MHLogicEventID::ServerConnectStart";
    case MHLogicEventID::ServerConnectSucceeded:
        return "MHLogicEventID::ServerConnectSucceeded";
    case MHLogicEventID::ServerConnectFailed:
        return "MHLogicEventID::ServerConnectFailed";
    case MHLogicEventID::PingResult:
        return "MHLogicEventID::PingResult";
    case MHLogicEventID::Disconnected:
        return "MHLogicEventID::Disconnected";
    case MHLogicEventID::EnterInstanceResponse:
        return "MHLogicEventID::EnterInstanceResponse";
    case MHLogicEventID::InstanceVerifyResponse:
        return "MHLogicEventID::InstanceVerifyResponse";
    case MHLogicEventID::GlobalErrcode:
        return "MHLogicEventID::GlobalErrcode";
    case MHLogicEventID::CheckVersionResponse:
        return "MHLogicEventID::CheckVersionResponse";
    case MHLogicEventID::RoleListResponse:
        return "MHLogicEventID::RoleListResponse";
    case MHLogicEventID::SelectRoleResponse:
        return "MHLogicEventID::SelectRoleResponse";
    case MHLogicEventID::TeamAddRoleResponse:
        return "MHLogicEventID::TeamAddRoleResponse";
    case MHLogicEventID::TeamUpdataRoleResponse:
        return "MHLogicEventID::TeamUpdataRoleResponse";
    case MHLogicEventID::PlayerTaskAcceptResponse:
        return "MHLogicEventID::PlayerTaskAcceptResponse";
    case MHLogicEventID::PlayerTaskGiveUpResponse:
        return "MHLogicEventID::PlayerTaskGiveUpResponse";
    case MHLogicEventID::PVPScoreResponse:
        return "MHLogicEventID::PVPScoreResponse";
    case MHLogicEventID::PVPEventResponse:
        return "MHLogicEventID::PVPEventResponse";
    case MHLogicEventID::PlayerTaskUIResetRespone:
        return "MHLogicEventID::PlayerTaskUIResetRespone";
    case MHLogicEventID::TeamInviteAsk:
        return "MHLogicEventID::TeamInviteAsk";
    case MHLogicEventID::TeamInviteRsp:
        return "MHLogicEventID::TeamInviteRsp";
    case MHLogicEventID::TeamJoinRsp:
        return "MHLogicEventID::TeamJoinRsp";
    case MHLogicEventID::NpcTaskUIResetRespone:
        return "MHLogicEventID::NpcTaskUIResetRespone";
    case MHLogicEventID::TeamLeaderNtf:
        return "MHLogicEventID::TeamLeaderNtf";
    case MHLogicEventID::TaskComplete:
        return "MHLogicEventID::TaskComplete";
    case MHLogicEventID::UpdateInventoryItemNtf:
        return "MHLogicEventID::UpdateInventoryItemNtf";
    case MHLogicEventID::UpdateInventoryInfoNtf:
        return "MHLogicEventID::UpdateInventoryInfoNtf";
    case MHLogicEventID::UpdateAvatarInfo:
        return "MHLogicEventID::UpdateAvatarInfo";
    case MHLogicEventID::ShowInfo:
        return "MHLogicEventID::ShowInfo";
    case MHLogicEventID::ChatMessage:
        return "MHLogicEventID::ChatMessage";
    case MHLogicEventID::UIWidgets:
        return "MHLogicEventID::UIWidgets";
    case MHLogicEventID::UIStageMode:
        return "MHLogicEventID::UIStageMode";
    case MHLogicEventID::UpdateNpcTalkContentNtf:
        return "MHLogicEventID::UpdateNpcTalkContentNtf";
    case MHLogicEventID::NpcTalkTitleListUpdate:
        return "MHLogicEventID::NpcTalkTitleListUpdate";
    case MHLogicEventID::PlayerTaskUpdte:
        return "MHLogicEventID::PlayerTaskUpdte";
    case MHLogicEventID::PlayerTaskTargetUpdte:
        return "MHLogicEventID::PlayerTaskTargetUpdte";
    case MHLogicEventID::PlayerFixedAwardItemUpdte:
        return "MHLogicEventID::PlayerFixedAwardItemUpdte";
    case MHLogicEventID::PlayerSelectedAwardItemUpdte:
        return "MHLogicEventID::PlayerSelectedAwardItemUpdte";
    case MHLogicEventID::PlayerOtherInfoUpdte:
        return "MHLogicEventID::PlayerOtherInfoUpdte";
    case MHLogicEventID::PlayerTaskTrackUpdte:
        return "MHLogicEventID::PlayerTaskTrackUpdte";
    case MHLogicEventID::NpcTaskTargetUpdte:
        return "MHLogicEventID::NpcTaskTargetUpdte";
    case MHLogicEventID::NpcFixedAwardItemUpdte:
        return "MHLogicEventID::NpcFixedAwardItemUpdte";
    case MHLogicEventID::IME_Ack:
        return "MHLogicEventID::IME_Ack";
    case MHLogicEventID::NpcOtherInfoUpdte:
        return "MHLogicEventID::NpcOtherInfoUpdte";
    case MHLogicEventID::MinimapSelfPlayerUpdate:
        return "MHLogicEventID::MinimapSelfPlayerUpdate";
    case MHLogicEventID::MinimapOtherPlayerUpdate:
        return "MHLogicEventID::MinimapOtherPlayerUpdate";
    case MHLogicEventID::MinimapItemUpdate:
        return "MHLogicEventID::MinimapItemUpdate";
    case MHLogicEventID::MiniMapNpcUpdate:
        return "MHLogicEventID::MiniMapNpcUpdate";
    case MHLogicEventID::MiniMapInfoUpdate:
        return "MHLogicEventID::MiniMapInfoUpdate";
    case MHLogicEventID::UpdateActionSlotID:
        return "MHLogicEventID::UpdateActionSlotID";
    case MHLogicEventID::UIInputAction:
        return "MHLogicEventID::UIInputAction";
    case MHLogicEventID::ChatLog_Enter:
        return "MHLogicEventID::ChatLog_Enter";
    case MHLogicEventID::ChatLabelUpdata:
        return "MHLogicEventID::ChatLabelUpdata";
    case MHLogicEventID::UpdateTeamInfo:
        return "MHLogicEventID::UpdateTeamInfo";
    case MHLogicEventID::UpdateTeamMemberInfo:
        return "MHLogicEventID::UpdateTeamMemberInfo";
    case MHLogicEventID::DelTeamMemberInfo:
        return "MHLogicEventID::DelTeamMemberInfo";
    case MHLogicEventID::UpdateSelfGreenHp:
        return "MHLogicEventID::UpdateSelfGreenHp";
    case MHLogicEventID::UpdateSelfRedHp:
        return "MHLogicEventID::UpdateSelfRedHp";
    case MHLogicEventID::UpdateForgeItem:
        return "MHLogicEventID::UpdateForgeItem";
    case MHLogicEventID::UpdateForgeMat:
        return "MHLogicEventID::UpdateForgeMat";
    case MHLogicEventID::UpdateItemCostAndIsForge:
        return "MHLogicEventID::UpdateItemCostAndIsForge";
    case MHLogicEventID::UpdateMainHudData:
        return "MHLogicEventID::UpdateMainHudData";
    case MHLogicEventID::ShowMessgaeBox:
        return "MHLogicEventID::ShowMessgaeBox";
    case MHLogicEventID::exitTeam:
        return "MHLogicEventID::exitTeam";
    case MHLogicEventID::UpdateSelfStamina:
        return "MHLogicEventID::UpdateSelfStamina";
    case MHLogicEventID::ChangeTeamLeader:
        return "MHLogicEventID::ChangeTeamLeader";
    case MHLogicEventID::PlayerBaseInfo:
        return "MHLogicEventID::PlayerBaseInfo";
    case MHLogicEventID::PlayerNameInfo:
        return "MHLogicEventID::PlayerNameInfo";
    case MHLogicEventID::InstanceMainInfo:
        return "MHLogicEventID::InstanceMainInfo";
    case MHLogicEventID::InstanceModeList:
        return "MHLogicEventID::InstanceModeList";
    case MHLogicEventID::SubInstanceInfo:
        return "MHLogicEventID::SubInstanceInfo";
    case MHLogicEventID::SubInstanceMonsterInfo:
        return "MHLogicEventID::SubInstanceMonsterInfo";
    case MHLogicEventID::StartCountDown:
        return "MHLogicEventID::StartCountDown";
    case MHLogicEventID::FailEnterInfo:
        return "MHLogicEventID::FailEnterInfo";
    case MHLogicEventID::InstanceComplete:
        return "MHLogicEventID::InstanceComplete";
    case MHLogicEventID::EnterInstanceInterrupt:
        return "MHLogicEventID::EnterInstanceInterrupt";
    case MHLogicEventID::Instance_Evaluate:
        return "MHLogicEventID::Instance_Evaluate";
    case MHLogicEventID::Instance_SettleAccount:
        return "MHLogicEventID::Instance_SettleAccount";
    case MHLogicEventID::Instance_InstanceOperation:
        return "MHLogicEventID::Instance_InstanceOperation";
    case MHLogicEventID::Instance_Next:
        return "MHLogicEventID::Instance_Next";
    case MHLogicEventID::Instance_TeamPlayerInfo:
        return "MHLogicEventID::Instance_TeamPlayerInfo";
    case MHLogicEventID::ShowDamageFloat:
        return "MHLogicEventID::ShowDamageFloat";
    case MHLogicEventID::UpdateBulletNum:
        return "MHLogicEventID::UpdateBulletNum";
    case MHLogicEventID::BulletReloadEnd:
        return "MHLogicEventID::BulletReloadEnd";
    case MHLogicEventID::SupplyItemUpate:
        return "MHLogicEventID::SupplyItemUpate";
    case MHLogicEventID::PushPromptMessage:
        return "MHLogicEventID::PushPromptMessage";
    case MHLogicEventID::UpdateBladeSharpness:
        return "MHLogicEventID::UpdateBladeSharpness";
    case MHLogicEventID::UpdateRageValue:
        return "MHLogicEventID::UpdateRageValue";
    case MHLogicEventID::InitForgeViewData:
        return "MHLogicEventID::InitForgeViewData";
    case MHLogicEventID::UpdateTachiEnergy:
        return "MHLogicEventID::UpdateTachiEnergy";
    case MHLogicEventID::PlayAnimation:
        return "MHLogicEventID::PlayAnimation";
    case MHLogicEventID::UpdatePlayerState:
        return "MHLogicEventID::UpdatePlayerState";
    case MHLogicEventID::ShowRightMenu:
        return "MHLogicEventID::ShowRightMenu";
    case MHLogicEventID::ShowMainMenu:
        return "MHLogicEventID::ShowMainMenu";
    case MHLogicEventID::QuestAddSucceed:
        return "MHLogicEventID::QuestAddSucceed";
    case MHLogicEventID::CraftSuccess:
        return "MHLogicEventID::CraftSuccess";
    case MHLogicEventID::ToolTipsGetData:
        return "MHLogicEventID::ToolTipsGetData";
    case MHLogicEventID::InitSystemConfig:
        return "MHLogicEventID::InitSystemConfig";
    case MHLogicEventID::GetStaticItemData:
        return "MHLogicEventID::GetStaticItemData";
    case MHLogicEventID::SupplyBoxInit:
        return "MHLogicEventID::SupplyBoxInit";
    case MHLogicEventID::SupplyBoxGetRsp:
        return "MHLogicEventID::SupplyBoxGetRsp";
    case MHLogicEventID::ShowOrHideLock:
        return "MHLogicEventID::ShowOrHideLock";
    case MHLogicEventID::UpdateLockState:
        return "MHLogicEventID::UpdateLockState";
    case MHLogicEventID::ShowOrHideTargetLock:
        return "MHLogicEventID::ShowOrHideTargetLock";
    case MHLogicEventID::ChangeLoginBtnState:
        return "MHLogicEventID::ChangeLoginBtnState";
    case MHLogicEventID::GetLoginLastAccount:
        return "MHLogicEventID::GetLoginLastAccount";
    case MHLogicEventID::ResponseNpcSaleInfo:
        return "MHLogicEventID::ResponseNpcSaleInfo";
    case MHLogicEventID::ResponseItemIconItemInfo:
        return "MHLogicEventID::ResponseItemIconItemInfo";
    case MHLogicEventID::ItemOperateRsp:
        return "MHLogicEventID::ItemOperateRsp";
    case MHLogicEventID::ItemInit:
        return "MHLogicEventID::ItemInit";
    case MHLogicEventID::OnLocalLogicPlayerEnter:
        return "MHLogicEventID::OnLocalLogicPlayerEnter";
    case MHLogicEventID::OnShoppingOperationResult:
        return "MHLogicEventID::OnShoppingOperationResult";
    case MHLogicEventID::NpcTalkTitleList_OtherInfo:
        return "MHLogicEventID::NpcTalkTitleList_OtherInfo";
    case MHLogicEventID::UpdateManufactureListInfo:
        return "MHLogicEventID::UpdateManufactureListInfo";
    case MHLogicEventID::ManufactureDetailedInfo:
        return "MHLogicEventID::ManufactureDetailedInfo";
    case MHLogicEventID::UpdateManufactureProficiency:
        return "MHLogicEventID::UpdateManufactureProficiency";
    case MHLogicEventID::UpdateManufactureStudyListInfo:
        return "MHLogicEventID::UpdateManufactureStudyListInfo";
    case MHLogicEventID::ManufactureMaterialInfo:
        return "MHLogicEventID::ManufactureMaterialInfo";
    case MHLogicEventID::UpdateAvailableSlot:
        return "MHLogicEventID::UpdateAvailableSlot";
    case MHLogicEventID::CreateDialogFromCPP:
        return "MHLogicEventID::CreateDialogFromCPP";
    case MHLogicEventID::GetNpcSaleInfoBySaleID:
        return "MHLogicEventID::GetNpcSaleInfoBySaleID";
    case MHLogicEventID::ShowIntoneBar:
        return "MHLogicEventID::ShowIntoneBar";
    case MHLogicEventID::UpdateExpandSlotCost:
        return "MHLogicEventID::UpdateExpandSlotCost";
    case MHLogicEventID::TradeStart:
        return "MHLogicEventID::TradeStart";
    case MHLogicEventID::TradeupdataInfo:
        return "MHLogicEventID::TradeupdataInfo";
    case MHLogicEventID::TradeClose:
        return "MHLogicEventID::TradeClose";
    case MHLogicEventID::TradeLock:
        return "MHLogicEventID::TradeLock";
    case MHLogicEventID::TradeComplete:
        return "MHLogicEventID::TradeComplete";
    case MHLogicEventID::TradeItemHandler:
        return "MHLogicEventID::TradeItemHandler";
    case MHLogicEventID::TradeupdataItem:
        return "MHLogicEventID::TradeupdataItem";
    case MHLogicEventID::ActionPointDataInit:
        return "MHLogicEventID::ActionPointDataInit";
    case MHLogicEventID::ActionPointDataUpdate:
        return "MHLogicEventID::ActionPointDataUpdate";
    case MHLogicEventID::ActionPointSelected:
        return "MHLogicEventID::ActionPointSelected";
    case MHLogicEventID::UpdateCharExp:
        return "MHLogicEventID::UpdateCharExp";
    case MHLogicEventID::UIInfo:
        return "MHLogicEventID::UIInfo";
    case MHLogicEventID::OtherTeamListInfo:
        return "MHLogicEventID::OtherTeamListInfo";
    case MHLogicEventID::OtherTeamMemberInfo:
        return "MHLogicEventID::OtherTeamMemberInfo";
    case MHLogicEventID::LevelGroupInfo:
        return "MHLogicEventID::LevelGroupInfo";
    case MHLogicEventID::InstanceClearingReadyCountDownStart:
        return "MHLogicEventID::InstanceClearingReadyCountDownStart";
    case MHLogicEventID::InstanceClearingReadyCountDownCompleted:
        return "MHLogicEventID::InstanceClearingReadyCountDownCompleted";
    case MHLogicEventID::EquipRankLevelUPResult:
        return "MHLogicEventID::EquipRankLevelUPResult";
    case MHLogicEventID::EquipRankLevelup2Six:
        return "MHLogicEventID::EquipRankLevelup2Six";
    case MHLogicEventID::PlayerInteractBegin:
        return "MHLogicEventID::PlayerInteractBegin";
    case MHLogicEventID::InstanceFinishMsg:
        return "MHLogicEventID::InstanceFinishMsg";
    case MHLogicEventID::InstancePlayerScore:
        return "MHLogicEventID::InstancePlayerScore";
    case MHLogicEventID::InstanceRecvResult:
        return "MHLogicEventID::InstanceRecvResult";
    case MHLogicEventID::InstancePlayerExpReward:
        return "MHLogicEventID::InstancePlayerExpReward";
    case MHLogicEventID::InstancePlayerGoldReward:
        return "MHLogicEventID::InstancePlayerGoldReward";
    case MHLogicEventID::AcceptTask:
        return "MHLogicEventID::AcceptTask";
    case MHLogicEventID::GiveupTask:
        return "MHLogicEventID::GiveupTask";
    case MHLogicEventID::SubmitTask:
        return "MHLogicEventID::SubmitTask";
    case MHLogicEventID::ShareTask:
        return "MHLogicEventID::ShareTask";
    case MHLogicEventID::TaskErr:
        return "MHLogicEventID::TaskErr";
    case MHLogicEventID::TaskContentExec:
        return "MHLogicEventID::TaskContentExec";
    case MHLogicEventID::TaskExec:
        return "MHLogicEventID::TaskExec";
    case MHLogicEventID::TaskContentStateChange:
        return "MHLogicEventID::TaskContentStateChange";
    case MHLogicEventID::TaskStateChange:
        return "MHLogicEventID::TaskStateChange";
    case MHLogicEventID::TaskLoad:
        return "MHLogicEventID::TaskLoad";
    case MHLogicEventID::SyncDaily:
        return "MHLogicEventID::SyncDaily";
    case MHLogicEventID::RefreshDaily:
        return "MHLogicEventID::RefreshDaily";
    case MHLogicEventID::RefreshSchedule:
        return "MHLogicEventID::RefreshSchedule";
    case MHLogicEventID::RefreshxDaily:
        return "MHLogicEventID::RefreshxDaily";
    case MHLogicEventID::InstanceInitEntryInfo:
        return "MHLogicEventID::InstanceInitEntryInfo";
    case MHLogicEventID::PlayerInteractEnd:
        return "MHLogicEventID::PlayerInteractEnd";
    case MHLogicEventID::TipsEquipAdditionalInfo:
        return "MHLogicEventID::TipsEquipAdditionalInfo";
    case MHLogicEventID::UpdateFriendList:
        return "MHLogicEventID::UpdateFriendList";
    case MHLogicEventID::UpdateFriendDetailInfo:
        return "MHLogicEventID::UpdateFriendDetailInfo";
    case MHLogicEventID::UpdateManuSkillInfo:
        return "MHLogicEventID::UpdateManuSkillInfo";
    case MHLogicEventID::ChangeTownServer:
        return "MHLogicEventID::ChangeTownServer";
    case MHLogicEventID::EquipEnforceBoard:
        return "MHLogicEventID::EquipEnforceBoard";
    case MHLogicEventID::UpdateLevelScoreAvatar:
        return "MHLogicEventID::UpdateLevelScoreAvatar";
    case MHLogicEventID::SetInstanceBoxData:
        return "MHLogicEventID::SetInstanceBoxData";
    case MHLogicEventID::ActionBoardDownloadComplete:
        return "MHLogicEventID::ActionBoardDownloadComplete";
    case MHLogicEventID::PerfectReloadInput:
        return "MHLogicEventID::PerfectReloadInput";
    case MHLogicEventID::BeginAmmoReload:
        return "MHLogicEventID::BeginAmmoReload";
    case MHLogicEventID::KeyMapInputNotifyUI:
        return "MHLogicEventID::KeyMapInputNotifyUI";
    case MHLogicEventID::InstanceUpdateRewardScore:
        return "MHLogicEventID::InstanceUpdateRewardScore";
    case MHLogicEventID::ResetBulletTab:
        return "MHLogicEventID::ResetBulletTab";
    case MHLogicEventID::UpdateInstanceAssess:
        return "MHLogicEventID::UpdateInstanceAssess";
    case MHLogicEventID::AvailableAmmoTypeChange:
        return "MHLogicEventID::AvailableAmmoTypeChange";
    case MHLogicEventID::AmmoSelectUIHide:
        return "MHLogicEventID::AmmoSelectUIHide";
    case MHLogicEventID::AddBuffer:
        return "MHLogicEventID::AddBuffer";
    case MHLogicEventID::DeleteBuffer:
        return "MHLogicEventID::DeleteBuffer";
    case MHLogicEventID::MouseMode:
        return "MHLogicEventID::MouseMode";
    case MHLogicEventID::UpdateAvatarImageMainUI:
        return "MHLogicEventID::UpdateAvatarImageMainUI";
    case MHLogicEventID::HideMessageBox:
        return "MHLogicEventID::HideMessageBox";
    case MHLogicEventID::EnablePlayerTaskDlgMouseClick:
        return "MHLogicEventID::EnablePlayerTaskDlgMouseClick";
    case MHLogicEventID::UpdateCatCarNumber:
        return "MHLogicEventID::UpdateCatCarNumber";
    case MHLogicEventID::UpdateAvatarToMainIcon:
        return "MHLogicEventID::UpdateAvatarToMainIcon";
    case MHLogicEventID::TradeAddItemError:
        return "MHLogicEventID::TradeAddItemError";
    case MHLogicEventID::EnableTidyBtn:
        return "MHLogicEventID::EnableTidyBtn";
    case MHLogicEventID::UpdateClockStatus:
        return "MHLogicEventID::UpdateClockStatus";
    case MHLogicEventID::UpdateBuffer:
        return "MHLogicEventID::UpdateBuffer";
    case MHLogicEventID::UpdateLevelName:
        return "MHLogicEventID::UpdateLevelName";
    case MHLogicEventID::ShowPlayerFrameAvatarFrame:
        return "MHLogicEventID::ShowPlayerFrameAvatarFrame";
    case MHLogicEventID::ManufactureSucceed:
        return "MHLogicEventID::ManufactureSucceed";
    case MHLogicEventID::OnCtrlC:
        return "MHLogicEventID::OnCtrlC";
    case MHLogicEventID::OnCtrlV:
        return "MHLogicEventID::OnCtrlV";
    case MHLogicEventID::ShowWaitQueue:
        return "MHLogicEventID::ShowWaitQueue";
    case MHLogicEventID::ItemMoveSwapUpdate:
        return "MHLogicEventID::ItemMoveSwapUpdate";
    case MHLogicEventID::GameChatMessage:
        return "MHLogicEventID::GameChatMessage";
    case MHLogicEventID::OnRequireExitGame:
        return "MHLogicEventID::OnRequireExitGame";
    case MHLogicEventID::PackItemList:
        return "MHLogicEventID::PackItemList";
    case MHLogicEventID::InstanceFinishNotify:
        return "MHLogicEventID::InstanceFinishNotify";
    case MHLogicEventID::RefreshDailyStatus:
        return "MHLogicEventID::RefreshDailyStatus";
    case MHLogicEventID::ObtainScheduleRewardList:
        return "MHLogicEventID::ObtainScheduleRewardList";
    case MHLogicEventID::ObtainScheduleTargetList:
        return "MHLogicEventID::ObtainScheduleTargetList";
    case MHLogicEventID::ScheduleError:
        return "MHLogicEventID::ScheduleError";
    case MHLogicEventID::ScheduleFetchPrize:
        return "MHLogicEventID::ScheduleFetchPrize";
    case MHLogicEventID::UpdateMailList:
        return "MHLogicEventID::UpdateMailList";
    case MHLogicEventID::RefreshMailDetail:
        return "MHLogicEventID::RefreshMailDetail";
    case MHLogicEventID::SendMailResponse:
        return "MHLogicEventID::SendMailResponse";
    case MHLogicEventID::DeleteMailResponse:
        return "MHLogicEventID::DeleteMailResponse";
    case MHLogicEventID::RequireMailList:
        return "MHLogicEventID::RequireMailList";
    case MHLogicEventID::FarmSwich:
        return "MHLogicEventID::FarmSwich";
    case MHLogicEventID::FarmOwner:
        return "MHLogicEventID::FarmOwner";
    case MHLogicEventID::UpdateBowEnergy:
        return "MHLogicEventID::UpdateBowEnergy";
    case MHLogicEventID::ReturnMailResponse:
        return "MHLogicEventID::ReturnMailResponse";
    case MHLogicEventID::ShowOrHideVehicleSight:
        return "MHLogicEventID::ShowOrHideVehicleSight";
    case MHLogicEventID::FarmUpgradeResponse:
        return "MHLogicEventID::FarmUpgradeResponse";
    case MHLogicEventID::ShowFarmNoticeHud:
        return "MHLogicEventID::ShowFarmNoticeHud";
    case MHLogicEventID::GuildErr:
        return "MHLogicEventID::GuildErr";
    case MHLogicEventID::GuildNotify:
        return "MHLogicEventID::GuildNotify";
    case MHLogicEventID::GetGuilds:
        return "MHLogicEventID::GetGuilds";
    case MHLogicEventID::FindGuilds:
        return "MHLogicEventID::FindGuilds";
    case MHLogicEventID::GetGuildApplys:
        return "MHLogicEventID::GetGuildApplys";
    case MHLogicEventID::GetGuildInvitations:
        return "MHLogicEventID::GetGuildInvitations";
    case MHLogicEventID::GetGuildDetail:
        return "MHLogicEventID::GetGuildDetail";
    case MHLogicEventID::GetGuildTitles:
        return "MHLogicEventID::GetGuildTitles";
    case MHLogicEventID::GetLeader:
        return "MHLogicEventID::GetLeader";
    case MHLogicEventID::GetSelf:
        return "MHLogicEventID::GetSelf";
    case MHLogicEventID::GetGuilderDetail:
        return "MHLogicEventID::GetGuilderDetail";
    case MHLogicEventID::ClearGuilderDetail:
        return "MHLogicEventID::ClearGuilderDetail";
    case MHLogicEventID::GetGuilder:
        return "MHLogicEventID::GetGuilder";
    case MHLogicEventID::GetGuildersCount:
        return "MHLogicEventID::GetGuildersCount";
    case MHLogicEventID::GetGuilders:
        return "MHLogicEventID::GetGuilders";
    case MHLogicEventID::GuildDepotFetchCountUpdate:
        return "MHLogicEventID::GuildDepotFetchCountUpdate";
    case MHLogicEventID::GuildRandCommoditiesUpdate:
        return "MHLogicEventID::GuildRandCommoditiesUpdate";
    case MHLogicEventID::GuildDepotOpCountUpdate:
        return "MHLogicEventID::GuildDepotOpCountUpdate";
    case MHLogicEventID::GetGuildQuest:
        return "MHLogicEventID::GetGuildQuest";
    case MHLogicEventID::UpdateDualBladesGhost:
        return "MHLogicEventID::UpdateDualBladesGhost";
    case MHLogicEventID::HunterStarAddCards:
        return "MHLogicEventID::HunterStarAddCards";
    case MHLogicEventID::PetItemTipsGetData:
        return "MHLogicEventID::PetItemTipsGetData";
    case MHLogicEventID::ShowHunterStarUIByCollection:
        return "MHLogicEventID::ShowHunterStarUIByCollection";
    case MHLogicEventID::UpdateNewBattleRemainTime:
        return "MHLogicEventID::UpdateNewBattleRemainTime";
    case MHLogicEventID::UpdateFarmInfo:
        return "MHLogicEventID::UpdateFarmInfo";
    case MHLogicEventID::UpdatePetList:
        return "MHLogicEventID::UpdatePetList";
    case MHLogicEventID::NotifyPetNameChange:
        return "MHLogicEventID::NotifyPetNameChange";
    case MHLogicEventID::NotifyPetDetailInfo:
        return "MHLogicEventID::NotifyPetDetailInfo";
    case MHLogicEventID::NotifyPetSkillLearn:
        return "MHLogicEventID::NotifyPetSkillLearn";
    case MHLogicEventID::NotifyPetEquipOnOff:
        return "MHLogicEventID::NotifyPetEquipOnOff";
    case MHLogicEventID::NotifyPetSkillEquipOnOff:
        return "MHLogicEventID::NotifyPetSkillEquipOnOff";
    case MHLogicEventID::UpdateLevelTrackData:
        return "MHLogicEventID::UpdateLevelTrackData";
    case MHLogicEventID::UpdateEntrustTrackState:
        return "MHLogicEventID::UpdateEntrustTrackState";
    case MHLogicEventID::FarmFacilityLevelUp:
        return "MHLogicEventID::FarmFacilityLevelUp";
    case MHLogicEventID::EntrustListUpdate:
        return "MHLogicEventID::EntrustListUpdate";
    case MHLogicEventID::NotifyPetAvatarUpdate:
        return "MHLogicEventID::NotifyPetAvatarUpdate";
    case MHLogicEventID::NotifyForgeTargetItem:
        return "MHLogicEventID::NotifyForgeTargetItem";
    case MHLogicEventID::TalkErr:
        return "MHLogicEventID::TalkErr";
    case MHLogicEventID::EntrustGoChangeMemState:
        return "MHLogicEventID::EntrustGoChangeMemState";
    case MHLogicEventID::EntrustGoMemSelPackage:
        return "MHLogicEventID::EntrustGoMemSelPackage";
    case MHLogicEventID::CatCuisineMakeSucceed:
        return "MHLogicEventID::CatCuisineMakeSucceed";
    case MHLogicEventID::FSM2UI:
        return "MHLogicEventID::FSM2UI";
    case MHLogicEventID::UpdateGiftPackData:
        return "MHLogicEventID::UpdateGiftPackData";
    case MHLogicEventID::LIneInfoNotify:
        return "MHLogicEventID::LIneInfoNotify";
    case MHLogicEventID::AllEntrustListUpdate:
        return "MHLogicEventID::AllEntrustListUpdate";
    case MHLogicEventID::ManufactureAddSkillDegree:
        return "MHLogicEventID::ManufactureAddSkillDegree";
    case MHLogicEventID::InstanceMonsterNewSize:
        return "MHLogicEventID::InstanceMonsterNewSize";
    case MHLogicEventID::UpdatePlayerProperty:
        return "MHLogicEventID::UpdatePlayerProperty";
    case MHLogicEventID::LocalPlayerDeadNotify:
        return "MHLogicEventID::LocalPlayerDeadNotify";
    case MHLogicEventID::MailSystemError:
        return "MHLogicEventID::MailSystemError";
    case MHLogicEventID::ChangeLevelNotifyUI:
        return "MHLogicEventID::ChangeLevelNotifyUI";
    case MHLogicEventID::PlaySoundSfx:
        return "MHLogicEventID::PlaySoundSfx";
    case MHLogicEventID::NewLineFullNotify:
        return "MHLogicEventID::NewLineFullNotify";
    case MHLogicEventID::ChatEnterNotifyUI:
        return "MHLogicEventID::ChatEnterNotifyUI";
    case MHLogicEventID::SchedulePrizeRefresh:
        return "MHLogicEventID::SchedulePrizeRefresh";
    case MHLogicEventID::OnEntrustAcceptOk:
        return "MHLogicEventID::OnEntrustAcceptOk";
    case MHLogicEventID::OnPvpAssignmentComplete:
        return "MHLogicEventID::OnPvpAssignmentComplete";
    case MHLogicEventID::OnPvpSpecialAward:
        return "MHLogicEventID::OnPvpSpecialAward";
    case MHLogicEventID::OnPvpScore:
        return "MHLogicEventID::OnPvpScore";
    case MHLogicEventID::UpdatePaidBoxItem:
        return "MHLogicEventID::UpdatePaidBoxItem";
    case MHLogicEventID::TeamPushVecNtf:
        return "MHLogicEventID::TeamPushVecNtf";
    case MHLogicEventID::CreateRelayConn:
        return "MHLogicEventID::CreateRelayConn";
    case MHLogicEventID::NpcAtdGiveGift:
        return "MHLogicEventID::NpcAtdGiveGift";
    case MHLogicEventID::NpcAtdErr:
        return "MHLogicEventID::NpcAtdErr";
    case MHLogicEventID::NpcAtdAddValue:
        return "MHLogicEventID::NpcAtdAddValue";
    case MHLogicEventID::ShowOrHideCommonProgress:
        return "MHLogicEventID::ShowOrHideCommonProgress";
    case MHLogicEventID::UpdateCommonProgressValue:
        return "MHLogicEventID::UpdateCommonProgressValue";
    case MHLogicEventID::PvpPlayerSetTrap:
        return "MHLogicEventID::PvpPlayerSetTrap";
    case MHLogicEventID::PvpPlayerKillPlayer:
        return "MHLogicEventID::PvpPlayerKillPlayer";
    case MHLogicEventID::PvpPlayerGetGodLuck:
        return "MHLogicEventID::PvpPlayerGetGodLuck";
    case MHLogicEventID::PvpGameOver:
        return "MHLogicEventID::PvpGameOver";
    case MHLogicEventID::PvpScoreRecording:
        return "MHLogicEventID::PvpScoreRecording";
    case MHLogicEventID::HunterStarFinishCard:
        return "MHLogicEventID::HunterStarFinishCard";
    case MHLogicEventID::UpdateMailUnReadNum:
        return "MHLogicEventID::UpdateMailUnReadNum";
    case MHLogicEventID::RequestNpcAtdInfoUpdate:
        return "MHLogicEventID::RequestNpcAtdInfoUpdate";
    case MHLogicEventID::RankListUpdate:
        return "MHLogicEventID::RankListUpdate";
    case MHLogicEventID::WeaponTalentLockStatusUpdate:
        return "MHLogicEventID::WeaponTalentLockStatusUpdate";
    case MHLogicEventID::WeaponTalentEquipTalentUpdate:
        return "MHLogicEventID::WeaponTalentEquipTalentUpdate";
    case MHLogicEventID::UpdateHammerStatus:
        return "MHLogicEventID::UpdateHammerStatus";
    case MHLogicEventID::ThousandLayerSupplyMemSel:
        return "MHLogicEventID::ThousandLayerSupplyMemSel";
    case MHLogicEventID::CDGroupStatus:
        return "MHLogicEventID::CDGroupStatus";
    case MHLogicEventID::RankErr:
        return "MHLogicEventID::RankErr";
    case MHLogicEventID::RankRewardFetched:
        return "MHLogicEventID::RankRewardFetched";
    case MHLogicEventID::WeaponTalentEquipProfoundSkill:
        return "MHLogicEventID::WeaponTalentEquipProfoundSkill";
    case MHLogicEventID::NpcAtdLevelUp:
        return "MHLogicEventID::NpcAtdLevelUp";
    case MHLogicEventID::NpcOrgLevelUp:
        return "MHLogicEventID::NpcOrgLevelUp";
    case MHLogicEventID::NpcAtdPreferUnlock:
        return "MHLogicEventID::NpcAtdPreferUnlock";
    case MHLogicEventID::WeaponTalentError:
        return "MHLogicEventID::WeaponTalentError";
    case MHLogicEventID::HunterStarRecordCardLevelUp:
        return "MHLogicEventID::HunterStarRecordCardLevelUp";
    case MHLogicEventID::UpdateGiantSwordStatus:
        return "MHLogicEventID::UpdateGiantSwordStatus";
    case MHLogicEventID::UpdatePetBattleInfo:
        return "MHLogicEventID::UpdatePetBattleInfo";
    case MHLogicEventID::LocalPlayerEnterGame:
        return "MHLogicEventID::LocalPlayerEnterGame";
    case MHLogicEventID::LocalPlayerSelectPlayAnimation:
        return "MHLogicEventID::LocalPlayerSelectPlayAnimation";
    case MHLogicEventID::EnableCreateRoleUI:
        return "MHLogicEventID::EnableCreateRoleUI";
    case MHLogicEventID::RefreshTitleList:
        return "MHLogicEventID::RefreshTitleList";
    case MHLogicEventID::RefreshTitleProperList:
        return "MHLogicEventID::RefreshTitleProperList";
    case MHLogicEventID::HunterStarBackSpaceEvent:
        return "MHLogicEventID::HunterStarBackSpaceEvent";
    case MHLogicEventID::DelAllBuffs:
        return "MHLogicEventID::DelAllBuffs";
    case MHLogicEventID::GuildNotifyToUI:
        return "MHLogicEventID::GuildNotifyToUI";
    case MHLogicEventID::SystemUnLockNotify:
        return "MHLogicEventID::SystemUnLockNotify";
    case MHLogicEventID::PlayerTaskUIShowNotify:
        return "MHLogicEventID::PlayerTaskUIShowNotify";
    case MHLogicEventID::OpenReviveUI:
        return "MHLogicEventID::OpenReviveUI";
    case MHLogicEventID::OnTradeBeginAsk:
        return "MHLogicEventID::OnTradeBeginAsk";
    case MHLogicEventID::FriendOnlineRemind:
        return "MHLogicEventID::FriendOnlineRemind";
    case MHLogicEventID::TeamInviteAndRequestMessagbox:
        return "MHLogicEventID::TeamInviteAndRequestMessagbox";
    case MHLogicEventID::BowBottleListUpdate:
        return "MHLogicEventID::BowBottleListUpdate";
    case MHLogicEventID::UpdateCatTicketNum:
        return "MHLogicEventID::UpdateCatTicketNum";
    case MHLogicEventID::UpdateReviveCoinNum:
        return "MHLogicEventID::UpdateReviveCoinNum";
    case MHLogicEventID::PlayerLevelChange:
        return "MHLogicEventID::PlayerLevelChange";
    case MHLogicEventID::DeathCameraPlayOver:
        return "MHLogicEventID::DeathCameraPlayOver";
    case MHLogicEventID::LocalPlayerAfterEnterBattleGround:
        return "MHLogicEventID::LocalPlayerAfterEnterBattleGround";
    case MHLogicEventID::CentreScreenInfoShow:
        return "MHLogicEventID::CentreScreenInfoShow";
    case MHLogicEventID::PetPropertyUpdateNotify:
        return "MHLogicEventID::PetPropertyUpdateNotify";
    case MHLogicEventID::UpdateGuildCampInfo:
        return "MHLogicEventID::UpdateGuildCampInfo";
    case MHLogicEventID::GuildCampLevelUp:
        return "MHLogicEventID::GuildCampLevelUp";
    case MHLogicEventID::SupplyBoxType:
        return "MHLogicEventID::SupplyBoxType";
    case MHLogicEventID::BattlePvpRemainTime:
        return "MHLogicEventID::BattlePvpRemainTime";
    case MHLogicEventID::PlayerRevive:
        return "MHLogicEventID::PlayerRevive";
    case MHLogicEventID::requestOpenPvpScoreList:
        return "MHLogicEventID::requestOpenPvpScoreList";
    case MHLogicEventID::localPlayerScoreAdded:
        return "MHLogicEventID::localPlayerScoreAdded";
    case MHLogicEventID::PvpHunterBoxOpen:
        return "MHLogicEventID::PvpHunterBoxOpen";
    case MHLogicEventID::PvpHunterBoxUpdate:
        return "MHLogicEventID::PvpHunterBoxUpdate";
    case MHLogicEventID::PvpHunterBoxClose:
        return "MHLogicEventID::PvpHunterBoxClose";
    case MHLogicEventID::PvpGroupSettlementUIOpen:
        return "MHLogicEventID::PvpGroupSettlementUIOpen";
    case MHLogicEventID::NewSkillNotifyToSkillVideoUi:
        return "MHLogicEventID::NewSkillNotifyToSkillVideoUi";
    case MHLogicEventID::MainHudBattleStateNotify:
        return "MHLogicEventID::MainHudBattleStateNotify";
    case MHLogicEventID::TalkExec:
        return "MHLogicEventID::TalkExec";
    case MHLogicEventID::FarmPetTraining:
        return "MHLogicEventID::FarmPetTraining";
    case MHLogicEventID::SensitiveVerify:
        return "MHLogicEventID::SensitiveVerify";
    case MHLogicEventID::SafeModeOpenUrlNotify:
        return "MHLogicEventID::SafeModeOpenUrlNotify";
    case MHLogicEventID::LimitUrlVerify:
        return "MHLogicEventID::LimitUrlVerify";
    case MHLogicEventID::GetManageUrl:
        return "MHLogicEventID::GetManageUrl";
    case MHLogicEventID::MapTypeNotify:
        return "MHLogicEventID::MapTypeNotify";
    case MHLogicEventID::DeadAnimPlayOver:
        return "MHLogicEventID::DeadAnimPlayOver";
    case MHLogicEventID::PvpScoreListRequirement:
        return "MHLogicEventID::PvpScoreListRequirement";
    case MHLogicEventID::LocalPlayerEnterPvp:
        return "MHLogicEventID::LocalPlayerEnterPvp";
    case MHLogicEventID::SameFactionPlayerEnterPvp:
        return "MHLogicEventID::SameFactionPlayerEnterPvp";
    case MHLogicEventID::PvpPlayerStatusChanged:
        return "MHLogicEventID::PvpPlayerStatusChanged";
    case MHLogicEventID::PvpPlayerBufferChanged:
        return "MHLogicEventID::PvpPlayerBufferChanged";
    case MHLogicEventID::PvpPlayerConnectionChanged:
        return "MHLogicEventID::PvpPlayerConnectionChanged";
    case MHLogicEventID::PvpHunterBoxJoin:
        return "MHLogicEventID::PvpHunterBoxJoin";
    case MHLogicEventID::ExternalWndCountUpdate:
        return "MHLogicEventID::ExternalWndCountUpdate";
    case MHLogicEventID::ChangeQTTalkState:
        return "MHLogicEventID::ChangeQTTalkState";
    case MHLogicEventID::SkillVideoUINotify:
        return "MHLogicEventID::SkillVideoUINotify";
    case MHLogicEventID::BattleGroundChanged:
        return "MHLogicEventID::BattleGroundChanged";
    case MHLogicEventID::InventoryChangedNotify:
        return "MHLogicEventID::InventoryChangedNotify";
    case MHLogicEventID::PVPStrikeKillingEvent:
        return "MHLogicEventID::PVPStrikeKillingEvent";
    case MHLogicEventID::BannerUpdate:
        return "MHLogicEventID::BannerUpdate";
    case MHLogicEventID::PvpPreparePlayerJoin:
        return "MHLogicEventID::PvpPreparePlayerJoin";
    case MHLogicEventID::PvpPreparePlayerLeave:
        return "MHLogicEventID::PvpPreparePlayerLeave";
    case MHLogicEventID::PvpPrepareStageStart:
        return "MHLogicEventID::PvpPrepareStageStart";
    case MHLogicEventID::PvpPrepareStageFinish:
        return "MHLogicEventID::PvpPrepareStageFinish";
    case MHLogicEventID::PvpPlayerGetScore:
        return "MHLogicEventID::PvpPlayerGetScore";
    case MHLogicEventID::PvpBattleRealStart:
        return "MHLogicEventID::PvpBattleRealStart";
    case MHLogicEventID::PvpBattleRealStartWithoutReadyGo:
        return "MHLogicEventID::PvpBattleRealStartWithoutReadyGo";
    case MHLogicEventID::GetGuildLearnedSkills:
        return "MHLogicEventID::GetGuildLearnedSkills";
    case MHLogicEventID::GetGuildCandidateSkills:
        return "MHLogicEventID::GetGuildCandidateSkills";
    case MHLogicEventID::PvpBattleShowFlashScoreBg:
        return "MHLogicEventID::PvpBattleShowFlashScoreBg";
    case MHLogicEventID::Pvp1stScoreStage:
        return "MHLogicEventID::Pvp1stScoreStage";
    case MHLogicEventID::VehicleAmmoExhaustChangeBullet:
        return "MHLogicEventID::VehicleAmmoExhaustChangeBullet";
    case MHLogicEventID::DismissClanNotify:
        return "MHLogicEventID::DismissClanNotify";
    case MHLogicEventID::QuitClanNotify:
        return "MHLogicEventID::QuitClanNotify";
    case MHLogicEventID::KickClanNotify:
        return "MHLogicEventID::KickClanNotify";
    case MHLogicEventID::OnGetClanersInfoNotify:
        return "MHLogicEventID::OnGetClanersInfoNotify";
    case MHLogicEventID::OnGetClanDetailNotify:
        return "MHLogicEventID::OnGetClanDetailNotify";
    case MHLogicEventID::OnCreateClaneError:
        return "MHLogicEventID::OnCreateClaneError";
    case MHLogicEventID::MartGoodsListRsp:
        return "MHLogicEventID::MartGoodsListRsp";
    case MHLogicEventID::ClanInviteRequire:
        return "MHLogicEventID::ClanInviteRequire";
    case MHLogicEventID::EnterCreateRole:
        return "MHLogicEventID::EnterCreateRole";
    case MHLogicEventID::LeaveCreateRole:
        return "MHLogicEventID::LeaveCreateRole";
    case MHLogicEventID::VehicleAmmoUpdate:
        return "MHLogicEventID::VehicleAmmoUpdate";
    case MHLogicEventID::OnGuildLeagueBeginAsk:
        return "MHLogicEventID::OnGuildLeagueBeginAsk";
    case MHLogicEventID::OnCreateClanOK:
        return "MHLogicEventID::OnCreateClanOK";
    case MHLogicEventID::OnGuildLeagueSignUpAgree:
        return "MHLogicEventID::OnGuildLeagueSignUpAgree";
    case MHLogicEventID::GuildDepotInit:
        return "MHLogicEventID::GuildDepotInit";
    case MHLogicEventID::GuildDepotNameUpdate:
        return "MHLogicEventID::GuildDepotNameUpdate";
    case MHLogicEventID::GuildItemOperate:
        return "MHLogicEventID::GuildItemOperate";
    case MHLogicEventID::GuildDepotSortBtnEnable:
        return "MHLogicEventID::GuildDepotSortBtnEnable";
    case MHLogicEventID::NetNotifyClanersRefresh:
        return "MHLogicEventID::NetNotifyClanersRefresh";
    case MHLogicEventID::PvpSameTeamGetScore:
        return "MHLogicEventID::PvpSameTeamGetScore";
    case MHLogicEventID::UpdateGuildInventoryItemNtf:
        return "MHLogicEventID::UpdateGuildInventoryItemNtf";
    case MHLogicEventID::GetOtherPlayerInfoNotify:
        return "MHLogicEventID::GetOtherPlayerInfoNotify";
    case MHLogicEventID::MainInstanceEnterType:
        return "MHLogicEventID::MainInstanceEnterType";
    case MHLogicEventID::PetTalentSkillRrefresh:
        return "MHLogicEventID::PetTalentSkillRrefresh";
    case MHLogicEventID::EquipUpgradeResult:
        return "MHLogicEventID::EquipUpgradeResult";
    case MHLogicEventID::IncDepotColumnCount:
        return "MHLogicEventID::IncDepotColumnCount";
    case MHLogicEventID::MartGoodBuyRsp:
        return "MHLogicEventID::MartGoodBuyRsp";
    case MHLogicEventID::TournamentGetRanksNotify:
        return "MHLogicEventID::TournamentGetRanksNotify";
    case MHLogicEventID::TournamentSearchPlayerNotify:
        return "MHLogicEventID::TournamentSearchPlayerNotify";
    case MHLogicEventID::UpdateTournamentRankList:
        return "MHLogicEventID::UpdateTournamentRankList";
    case MHLogicEventID::SpoorErr:
        return "MHLogicEventID::SpoorErr";
    case MHLogicEventID::SpoorFinish:
        return "MHLogicEventID::SpoorFinish";
    case MHLogicEventID::SpoorPrizeFetched:
        return "MHLogicEventID::SpoorPrizeFetched";
    case MHLogicEventID::SpoorNewPrize:
        return "MHLogicEventID::SpoorNewPrize";
    case MHLogicEventID::AddSpoorPoints:
        return "MHLogicEventID::AddSpoorPoints";
    case MHLogicEventID::WarningShow:
        return "MHLogicEventID::WarningShow";
    case MHLogicEventID::NewTaskAutoAccept:
        return "MHLogicEventID::NewTaskAutoAccept";
    case MHLogicEventID::SensitiveVerifyWithTips:
        return "MHLogicEventID::SensitiveVerifyWithTips";
    case MHLogicEventID::BuffItemNtf:
        return "MHLogicEventID::BuffItemNtf";
    case MHLogicEventID::ItemDecompose:
        return "MHLogicEventID::ItemDecompose";
    case MHLogicEventID::AuctionEventToUI:
        return "MHLogicEventID::AuctionEventToUI";
    case MHLogicEventID::ShortCutBarFlashByWidgetID:
        return "MHLogicEventID::ShortCutBarFlashByWidgetID";
    case MHLogicEventID::CommonGroupLocalPlayerEnter:
        return "MHLogicEventID::CommonGroupLocalPlayerEnter";
    case MHLogicEventID::CommonGroupPlayerStatusChanged:
        return "MHLogicEventID::CommonGroupPlayerStatusChanged";
    case MHLogicEventID::CommonGroupPlayerBufferChanged:
        return "MHLogicEventID::CommonGroupPlayerBufferChanged";
    case MHLogicEventID::CommonGroupConnectionChanged:
        return "MHLogicEventID::CommonGroupConnectionChanged";
    case MHLogicEventID::CommonGroupOtherPlayerEnter:
        return "MHLogicEventID::CommonGroupOtherPlayerEnter";
    case MHLogicEventID::RefreshScultUI:
        return "MHLogicEventID::RefreshScultUI";
    case MHLogicEventID::AirshipControlState:
        return "MHLogicEventID::AirshipControlState";
    case MHLogicEventID::RefreshTreasureList:
        return "MHLogicEventID::RefreshTreasureList";
    case MHLogicEventID::RefreshTreasureAwardList:
        return "MHLogicEventID::RefreshTreasureAwardList";
    case MHLogicEventID::FarmAdvGatherRsp:
        return "MHLogicEventID::FarmAdvGatherRsp";
    case MHLogicEventID::OnAirshipControlStateSelect:
        return "MHLogicEventID::OnAirshipControlStateSelect";
    case MHLogicEventID::CallFGPlayPetFireCutscene:
        return "MHLogicEventID::CallFGPlayPetFireCutscene";
    case MHLogicEventID::OpenPetFireConfirm:
        return "MHLogicEventID::OpenPetFireConfirm";
    case MHLogicEventID::UpdateEntrustMoney:
        return "MHLogicEventID::UpdateEntrustMoney";
    case MHLogicEventID::RefreshCreateRoleUIFromLocalData:
        return "MHLogicEventID::RefreshCreateRoleUIFromLocalData";
    case MHLogicEventID::UpdateSpecLevelInfo:
        return "MHLogicEventID::UpdateSpecLevelInfo";
    case MHLogicEventID::HornMessage:
        return "MHLogicEventID::HornMessage";
    case MHLogicEventID::BattWatchModeNotify:
        return "MHLogicEventID::BattWatchModeNotify";
    case MHLogicEventID::GuildMatchEventToUI:
        return "MHLogicEventID::GuildMatchEventToUI";
    case MHLogicEventID::SwitchWindowMode:
        return "MHLogicEventID::SwitchWindowMode";
    case MHLogicEventID::SystemNoticMsg:
        return "MHLogicEventID::SystemNoticMsg";
    case MHLogicEventID::RapidHuntHisListUpdate:
        return "MHLogicEventID::RapidHuntHisListUpdate";
    case MHLogicEventID::CallFGPlayEquipForgeCutscene:
        return "MHLogicEventID::CallFGPlayEquipForgeCutscene";
    case MHLogicEventID::EquipForgeCutsceneFinishedNotify:
        return "MHLogicEventID::EquipForgeCutsceneFinishedNotify";
    case MHLogicEventID::OpenVIPMsg:
        return "MHLogicEventID::OpenVIPMsg";
    case MHLogicEventID::OpenVIPGrowthMsg:
        return "MHLogicEventID::OpenVIPGrowthMsg";
    case MHLogicEventID::OpenVIPProfitMsg:
        return "MHLogicEventID::OpenVIPProfitMsg";
    case MHLogicEventID::VIPServiceExpireMsg:
        return "MHLogicEventID::VIPServiceExpireMsg";
    case MHLogicEventID::FeatureUpdated:
        return "MHLogicEventID::FeatureUpdated";
    case MHLogicEventID::ExchangeEventToUI:
        return "MHLogicEventID::ExchangeEventToUI";
    case MHLogicEventID::EnableOITTest:
        return "MHLogicEventID::EnableOITTest";
    case MHLogicEventID::GuildMatchErr:
        return "MHLogicEventID::GuildMatchErr";
    case MHLogicEventID::OpenMartVipPanelNotify:
        return "MHLogicEventID::OpenMartVipPanelNotify";
    case MHLogicEventID::ActivityAddAward:
        return "MHLogicEventID::ActivityAddAward";
    case MHLogicEventID::ActivityFetchAward:
        return "MHLogicEventID::ActivityFetchAward";
    case MHLogicEventID::ActivityRefreshAward:
        return "MHLogicEventID::ActivityRefreshAward";
    case MHLogicEventID::ActivityErr:
        return "MHLogicEventID::ActivityErr";
    case MHLogicEventID::HunterGroupPvpScoreNotify:
        return "MHLogicEventID::HunterGroupPvpScoreNotify";
    case MHLogicEventID::UpdateQuickMatchInfo:
        return "MHLogicEventID::UpdateQuickMatchInfo";
    case MHLogicEventID::ActivityAwardRefresh:
        return "MHLogicEventID::ActivityAwardRefresh";
    case MHLogicEventID::UIResolutionChangedNotify:
        return "MHLogicEventID::UIResolutionChangedNotify";
    case MHLogicEventID::LocalPlayerWeaponUnlock:
        return "MHLogicEventID::LocalPlayerWeaponUnlock";
    case MHLogicEventID::SwitchNVFur:
        return "MHLogicEventID::SwitchNVFur";
    case MHLogicEventID::OnAutoReviveEvent:
        return "MHLogicEventID::OnAutoReviveEvent";
    case MHLogicEventID::UpdatePetMonsterGreenHp:
        return "MHLogicEventID::UpdatePetMonsterGreenHp";
    case MHLogicEventID::UpdatePetMonsterRedHp:
        return "MHLogicEventID::UpdatePetMonsterRedHp";
    case MHLogicEventID::UpdatePetMonsterStamina:
        return "MHLogicEventID::UpdatePetMonsterStamina";
    case MHLogicEventID::UpdatePetMonsterRageValue:
        return "MHLogicEventID::UpdatePetMonsterRageValue";
    case MHLogicEventID::UpdatePetMonsterState:
        return "MHLogicEventID::UpdatePetMonsterState";
    case MHLogicEventID::UpdatePetMonsterMainInfo:
        return "MHLogicEventID::UpdatePetMonsterMainInfo";
    case MHLogicEventID::OpenMonsterHud:
        return "MHLogicEventID::OpenMonsterHud";
    case MHLogicEventID::MonsterIndexNotify:
        return "MHLogicEventID::MonsterIndexNotify";
    case MHLogicEventID::PetMonsterUIInfoNotify:
        return "MHLogicEventID::PetMonsterUIInfoNotify";
    case MHLogicEventID::PetMonsterSkillInfoUpdate:
        return "MHLogicEventID::PetMonsterSkillInfoUpdate";
    case MHLogicEventID::PetMonsterSkillUse:
        return "MHLogicEventID::PetMonsterSkillUse";
    case MHLogicEventID::UpdateCursorVisible:
        return "MHLogicEventID::UpdateCursorVisible";
    case MHLogicEventID::GetAFluteNote:
        return "MHLogicEventID::GetAFluteNote";
    case MHLogicEventID::GetMelodyEffect:
        return "MHLogicEventID::GetMelodyEffect";
    case MHLogicEventID::LostMelodyEffect:
        return "MHLogicEventID::LostMelodyEffect";
    case MHLogicEventID::RemoveMelody:
        return "MHLogicEventID::RemoveMelody";
    case MHLogicEventID::FluteOpernKeyClick:
        return "MHLogicEventID::FluteOpernKeyClick";
    case MHLogicEventID::FluteChooseOpern:
        return "MHLogicEventID::FluteChooseOpern";
    case MHLogicEventID::UpgradeFluteNote:
        return "MHLogicEventID::UpgradeFluteNote";
    case MHLogicEventID::GenerateMelody:
        return "MHLogicEventID::GenerateMelody";
    case MHLogicEventID::InvokeFluteTune:
        return "MHLogicEventID::InvokeFluteTune";
    case MHLogicEventID::WakePointNotfiy:
        return "MHLogicEventID::WakePointNotfiy";
    case MHLogicEventID::WakeStatusNotify:
        return "MHLogicEventID::WakeStatusNotify";
    case MHLogicEventID::WakeTimesResetNotify:
        return "MHLogicEventID::WakeTimesResetNotify";
    case MHLogicEventID::ShowLobbyUI:
        return "MHLogicEventID::ShowLobbyUI";
    case MHLogicEventID::RefreshMartCredit:
        return "MHLogicEventID::RefreshMartCredit";
    case MHLogicEventID::LookOtherTeamInfoRefresh:
        return "MHLogicEventID::LookOtherTeamInfoRefresh";
    case MHLogicEventID::UpdateReviveUI:
        return "MHLogicEventID::UpdateReviveUI";
    case MHLogicEventID::LineUpRetry:
        return "MHLogicEventID::LineUpRetry";
    case MHLogicEventID::GameCountDown:
        return "MHLogicEventID::GameCountDown";
    case MHLogicEventID::ItemExchangeResult:
        return "MHLogicEventID::ItemExchangeResult";
    case MHLogicEventID::Enable3DMode:
        return "MHLogicEventID::Enable3DMode";
    case MHLogicEventID::Ipvp_TeamScoreUpdate:
        return "MHLogicEventID::Ipvp_TeamScoreUpdate";
    case MHLogicEventID::Ipvp_PlayerScoreUpdate:
        return "MHLogicEventID::Ipvp_PlayerScoreUpdate";
    case MHLogicEventID::WeaponTraceInfoUpdate:
        return "MHLogicEventID::WeaponTraceInfoUpdate";
    case MHLogicEventID::InstanceSelect_responseServerUnlockLevel:
        return "MHLogicEventID::InstanceSelect_responseServerUnlockLevel";
    case MHLogicEventID::WeaponForgeSuccess:
        return "MHLogicEventID::WeaponForgeSuccess";
    case MHLogicEventID::CountDownNumberShow:
        return "MHLogicEventID::CountDownNumberShow";
    case MHLogicEventID::Ipvp_Settlement:
        return "MHLogicEventID::Ipvp_Settlement";
    case MHLogicEventID::UnlockPetSkillSlot:
        return "MHLogicEventID::UnlockPetSkillSlot";
    case MHLogicEventID::EquipTransferResult:
        return "MHLogicEventID::EquipTransferResult";
    case MHLogicEventID::FarmCDReduceResult:
        return "MHLogicEventID::FarmCDReduceResult";
    case MHLogicEventID::TransferResult:
        return "MHLogicEventID::TransferResult";
    case MHLogicEventID::UpdateTutorialQTE:
        return "MHLogicEventID::UpdateTutorialQTE";
    case MHLogicEventID::UpdateItemData:
        return "MHLogicEventID::UpdateItemData";
    case MHLogicEventID::OperationActivity_FileDownloaded:
        return "MHLogicEventID::OperationActivity_FileDownloaded";
    case MHLogicEventID::CallFGPlayPetEmployCutscene:
        return "MHLogicEventID::CallFGPlayPetEmployCutscene";
    case MHLogicEventID::WeaponTutorialWeaponPreview:
        return "MHLogicEventID::WeaponTutorialWeaponPreview";
    case MHLogicEventID::UpdateScriptActivity:
        return "MHLogicEventID::UpdateScriptActivity";
    case MHLogicEventID::RandomShopRefresh:
        return "MHLogicEventID::RandomShopRefresh";
    case MHLogicEventID::UpdateQueueInfo:
        return "MHLogicEventID::UpdateQueueInfo";
    case MHLogicEventID::TaskSubmitting:
        return "MHLogicEventID::TaskSubmitting";
    case MHLogicEventID::DragonBoxOpenEnd:
        return "MHLogicEventID::DragonBoxOpenEnd";
    case MHLogicEventID::DragonBoxDetail:
        return "MHLogicEventID::DragonBoxDetail";
    case MHLogicEventID::DragonBoxPrizeFetch:
        return "MHLogicEventID::DragonBoxPrizeFetch";
    case MHLogicEventID::DragonBoxArgs:
        return "MHLogicEventID::DragonBoxArgs";
    case MHLogicEventID::DragonBoxOpen:
        return "MHLogicEventID::DragonBoxOpen";
    case MHLogicEventID::PetTreasureDigResut:
        return "MHLogicEventID::PetTreasureDigResut";
    case MHLogicEventID::ChangeUIActionMode:
        return "MHLogicEventID::ChangeUIActionMode";
    case MHLogicEventID::TryPurchaseVip:
        return "MHLogicEventID::TryPurchaseVip";
    case MHLogicEventID::WinXinPayUrl:
        return "MHLogicEventID::WinXinPayUrl";
    case MHLogicEventID::VipUpdate:
        return "MHLogicEventID::VipUpdate";
    case MHLogicEventID::OpenMartView:
        return "MHLogicEventID::OpenMartView";
    case MHLogicEventID::RefreshTraceUINotity:
        return "MHLogicEventID::RefreshTraceUINotity";
    case MHLogicEventID::PlayerResetPositionStateChange:
        return "MHLogicEventID::PlayerResetPositionStateChange";
    case MHLogicEventID::PvpBattleListUpdate:
        return "MHLogicEventID::PvpBattleListUpdate";
    case MHLogicEventID::GuildSelectQualifierInstance:
        return "MHLogicEventID::GuildSelectQualifierInstance";
    case MHLogicEventID::BattleSupplyCatUnLock:
        return "MHLogicEventID::BattleSupplyCatUnLock";
    case MHLogicEventID::GunLanceUpdate:
        return "MHLogicEventID::GunLanceUpdate";
    case MHLogicEventID::ContestRankTitleUpdate:
        return "MHLogicEventID::ContestRankTitleUpdate";
    case MHLogicEventID::ContestRankTeamMatesListUpdate:
        return "MHLogicEventID::ContestRankTeamMatesListUpdate";
    case MHLogicEventID::ShowOrHideRadarProgress:
        return "MHLogicEventID::ShowOrHideRadarProgress";
    case MHLogicEventID::UpdateRadarProgressValue:
        return "MHLogicEventID::UpdateRadarProgressValue";
    case MHLogicEventID::ReportPlayerResult:
        return "MHLogicEventID::ReportPlayerResult";
    case MHLogicEventID::NewItemNotice:
        return "MHLogicEventID::NewItemNotice";
    case MHLogicEventID::MartRefresh:
        return "MHLogicEventID::MartRefresh";
    case MHLogicEventID::QuickOpenVIPMsg:
        return "MHLogicEventID::QuickOpenVIPMsg";
    case MHLogicEventID::IncomingNoticMsg:
        return "MHLogicEventID::IncomingNoticMsg";
    case MHLogicEventID::TaskTraceInfoUpdate:
        return "MHLogicEventID::TaskTraceInfoUpdate";
    case MHLogicEventID::CancelWeiXinOrderOk:
        return "MHLogicEventID::CancelWeiXinOrderOk";
    case MHLogicEventID::WeiXinOrderTimeout:
        return "MHLogicEventID::WeiXinOrderTimeout";
    case MHLogicEventID::UpdateFluteOphern:
        return "MHLogicEventID::UpdateFluteOphern";
    case MHLogicEventID::FluteStateReset:
        return "MHLogicEventID::FluteStateReset";
    case MHLogicEventID::LocalGameSettingUpdate:
        return "MHLogicEventID::LocalGameSettingUpdate";
    case MHLogicEventID::NetworkStatus:
        return "MHLogicEventID::NetworkStatus";
    case MHLogicEventID::FileCheckTimeOutReq:
        return "MHLogicEventID::FileCheckTimeOutReq";
    case MHLogicEventID::NotifyUIBake:
        return "MHLogicEventID::NotifyUIBake";
    case MHLogicEventID::IpvpAttackInfo:
        return "MHLogicEventID::IpvpAttackInfo";
    case MHLogicEventID::IpvpMiniMapEvent:
        return "MHLogicEventID::IpvpMiniMapEvent";
    case MHLogicEventID::YoukuReplayStatusNotify:
        return "MHLogicEventID::YoukuReplayStatusNotify";
    case MHLogicEventID::NpcTalkSendFGEvent:
        return "MHLogicEventID::NpcTalkSendFGEvent";
    case MHLogicEventID::RapidSpeak:
        return "MHLogicEventID::RapidSpeak";
    case MHLogicEventID::ScriptActivityLoaded:
        return "MHLogicEventID::ScriptActivityLoaded";
    case MHLogicEventID::ScriptActivityLoading:
        return "MHLogicEventID::ScriptActivityLoading";
    case MHLogicEventID::GamePadSwitchToUIMode:
        return "MHLogicEventID::GamePadSwitchToUIMode";
    case MHLogicEventID::PackItemPlayFx:
        return "MHLogicEventID::PackItemPlayFx";
    case MHLogicEventID::StarStoneFeedOk:
        return "MHLogicEventID::StarStoneFeedOk";
    case MHLogicEventID::StarStoneRepair:
        return "MHLogicEventID::StarStoneRepair";
    case MHLogicEventID::UpdateEquipColorList:
        return "MHLogicEventID::UpdateEquipColorList";
    case MHLogicEventID::UpdateEquipDyeView:
        return "MHLogicEventID::UpdateEquipDyeView";
    case MHLogicEventID::EquipDyeViewRespond:
        return "MHLogicEventID::EquipDyeViewRespond";
    case MHLogicEventID::StarStoneEnchaseOk:
        return "MHLogicEventID::StarStoneEnchaseOk";
    case MHLogicEventID::RefreshInstanceRewardView:
        return "MHLogicEventID::RefreshInstanceRewardView";
    case MHLogicEventID::WelfareMenuCanFetchStatusUpdate:
        return "MHLogicEventID::WelfareMenuCanFetchStatusUpdate";
    case MHLogicEventID::OperationActivityNoticeStatusUpdate:
        return "MHLogicEventID::OperationActivityNoticeStatusUpdate";
    case MHLogicEventID::FriendNotify:
        return "MHLogicEventID::FriendNotify";
    case MHLogicEventID::RefreshGiveupUI:
        return "MHLogicEventID::RefreshGiveupUI";
    case MHLogicEventID::NotifyGiveupUI:
        return "MHLogicEventID::NotifyGiveupUI";
    case MHLogicEventID::OnReceiveNewGift:
        return "MHLogicEventID::OnReceiveNewGift";
    case MHLogicEventID::ChangeEquipNotify:
        return "MHLogicEventID::ChangeEquipNotify";
    case MHLogicEventID::LevelCatLoot:
        return "MHLogicEventID::LevelCatLoot";
    case MHLogicEventID::FarmEquipAvatarGetResult:
        return "MHLogicEventID::FarmEquipAvatarGetResult";
    case MHLogicEventID::Mart_FileDownloaded:
        return "MHLogicEventID::Mart_FileDownloaded";
    case MHLogicEventID::UpdateEquipPlanList:
        return "MHLogicEventID::UpdateEquipPlanList";
    case MHLogicEventID::PreviewEquipPlan:
        return "MHLogicEventID::PreviewEquipPlan";
    case MHLogicEventID::GiveUpRevive:
        return "MHLogicEventID::GiveUpRevive";
    case MHLogicEventID::UpdateNewCharExpBar:
        return "MHLogicEventID::UpdateNewCharExpBar";
    case MHLogicEventID::PetMonsterSkillUseNotify:
        return "MHLogicEventID::PetMonsterSkillUseNotify";
    case MHLogicEventID::CloseMonsterHud:
        return "MHLogicEventID::CloseMonsterHud";
    case MHLogicEventID::MHUIMonsterSelector:
        return "MHLogicEventID::MHUIMonsterSelector";
    case MHLogicEventID::ChangeWeaponInPreparation:
        return "MHLogicEventID::ChangeWeaponInPreparation";
    case MHLogicEventID::GuildNameUpdate:
        return "MHLogicEventID::GuildNameUpdate";
    case MHLogicEventID::EquipTransmogrifyOk:
        return "MHLogicEventID::EquipTransmogrifyOk";
    case MHLogicEventID::AddTeamBuff:
        return "MHLogicEventID::AddTeamBuff";
    case MHLogicEventID::UpdateInstanceReward:
        return "MHLogicEventID::UpdateInstanceReward";
    case MHLogicEventID::VehicleFireCDNotify:
        return "MHLogicEventID::VehicleFireCDNotify";
    case MHLogicEventID::RequestQuenchEquipRsp:
        return "MHLogicEventID::RequestQuenchEquipRsp";
    case MHLogicEventID::MatchEntryUpdateRewardState:
        return "MHLogicEventID::MatchEntryUpdateRewardState";
    case MHLogicEventID::WildHuntGuildInfo:
        return "MHLogicEventID::WildHuntGuildInfo";
    case MHLogicEventID::WildHuntSignUpOk:
        return "MHLogicEventID::WildHuntSignUpOk";
    case MHLogicEventID::IsFaceEditing:
        return "MHLogicEventID::IsFaceEditing";
    case MHLogicEventID::BattleTeamNameUpdate:
        return "MHLogicEventID::BattleTeamNameUpdate";
    case MHLogicEventID::PetShopRefresh:
        return "MHLogicEventID::PetShopRefresh";
    case MHLogicEventID::AutoBacktoRolelist:
        return "MHLogicEventID::AutoBacktoRolelist";
    case MHLogicEventID::RefreshSupplyPlan:
        return "MHLogicEventID::RefreshSupplyPlan";
    case MHLogicEventID::SupplyPlanEquip:
        return "MHLogicEventID::SupplyPlanEquip";
    case MHLogicEventID::RecvLevelCatRewardList:
        return "MHLogicEventID::RecvLevelCatRewardList";
    case MHLogicEventID::RefreshNewEquipUpgrade:
        return "MHLogicEventID::RefreshNewEquipUpgrade";
    case MHLogicEventID::GetSailMoney:
        return "MHLogicEventID::GetSailMoney";
    case MHLogicEventID::EnablePerfDmp:
        return "MHLogicEventID::EnablePerfDmp";
    case MHLogicEventID::SuitSkillActiveOk:
        return "MHLogicEventID::SuitSkillActiveOk";
    case MHLogicEventID::SuitSkillEditOk:
        return "MHLogicEventID::SuitSkillEditOk";
    case MHLogicEventID::InstanceEnterModeUpdate:
        return "MHLogicEventID::InstanceEnterModeUpdate";
    case MHLogicEventID::AddLikeItOk:
        return "MHLogicEventID::AddLikeItOk";
    case MHLogicEventID::WaitSkillListUpdate:
        return "MHLogicEventID::WaitSkillListUpdate";
    case MHLogicEventID::PetStatusChangeNotify:
        return "MHLogicEventID::PetStatusChangeNotify";
    case MHLogicEventID::OnDivinationStarLightened:
        return "MHLogicEventID::OnDivinationStarLightened";
    case MHLogicEventID::OnDivinationAnswerFeedback:
        return "MHLogicEventID::OnDivinationAnswerFeedback";
    case MHLogicEventID::OnDivinationError:
        return "MHLogicEventID::OnDivinationError";
    case MHLogicEventID::OnDivinationGetSubject:
        return "MHLogicEventID::OnDivinationGetSubject";
    case MHLogicEventID::WaitSkillDeleteNotify:
        return "MHLogicEventID::WaitSkillDeleteNotify";
    case MHLogicEventID::HuntCreditCoinUsedUpdate:
        return "MHLogicEventID::HuntCreditCoinUsedUpdate";
    case MHLogicEventID::LevelRegionChangeNotify:
        return "MHLogicEventID::LevelRegionChangeNotify";
    case MHLogicEventID::OnQuenchSaveRsp:
        return "MHLogicEventID::OnQuenchSaveRsp";
    case MHLogicEventID::BossFornotice:
        return "MHLogicEventID::BossFornotice";
    case MHLogicEventID::RefreshThousandSupplyUI:
        return "MHLogicEventID::RefreshThousandSupplyUI";
    case MHLogicEventID::Loading_FileDownloaded:
        return "MHLogicEventID::Loading_FileDownloaded";
    case MHLogicEventID::WildHuntTaskUpdate:
        return "MHLogicEventID::WildHuntTaskUpdate";
    case MHLogicEventID::LogParticleInfo:
        return "MHLogicEventID::LogParticleInfo";
    case MHLogicEventID::OnPvMScore:
        return "MHLogicEventID::OnPvMScore";
    case MHLogicEventID::OneByOneChatMessage:
        return "MHLogicEventID::OneByOneChatMessage";
    case MHLogicEventID::SwitchPrivateChat:
        return "MHLogicEventID::SwitchPrivateChat";
    case MHLogicEventID::UpdateActionBarData:
        return "MHLogicEventID::UpdateActionBarData";
    case MHLogicEventID::ClearBuffIcon:
        return "MHLogicEventID::ClearBuffIcon";
    case MHLogicEventID::OnExpressionLvUp:
        return "MHLogicEventID::OnExpressionLvUp";
    case MHLogicEventID::UpdateRightMenuData:
        return "MHLogicEventID::UpdateRightMenuData";
    case MHLogicEventID::RefreshEquipRecast:
        return "MHLogicEventID::RefreshEquipRecast";
    case MHLogicEventID::UISplashOver:
        return "MHLogicEventID::UISplashOver";
    case MHLogicEventID::AddPopChat:
        return "MHLogicEventID::AddPopChat";
    case MHLogicEventID::WorldMap:
        return "MHLogicEventID::WorldMap";
    case MHLogicEventID::YamatuskamiView:
        return "MHLogicEventID::YamatuskamiView";
    case MHLogicEventID::MonolopyEvent:
        return "MHLogicEventID::MonolopyEvent";
    case MHLogicEventID::RefreshItemRemoveBind:
        return "MHLogicEventID::RefreshItemRemoveBind";
    case MHLogicEventID::NewbieVideoPlay:
        return "MHLogicEventID::NewbieVideoPlay";
    case MHLogicEventID::WildHuntSecretQuestTimeUpdate:
        return "MHLogicEventID::WildHuntSecretQuestTimeUpdate";
    case MHLogicEventID::OnNewbieVideoPlayEnd:
        return "MHLogicEventID::OnNewbieVideoPlayEnd";
    case MHLogicEventID::TransformSwordShapeOrValueChanged:
        return "MHLogicEventID::TransformSwordShapeOrValueChanged";
    case MHLogicEventID::SetTowerDefenseTemplateIdEvent:
        return "MHLogicEventID::SetTowerDefenseTemplateIdEvent";
    case MHLogicEventID::SetTowerDefenseTypeIdEvent:
        return "MHLogicEventID::SetTowerDefenseTypeIdEvent";
    case MHLogicEventID::RefreshTowerDefenseMainView:
        return "MHLogicEventID::RefreshTowerDefenseMainView";
    case MHLogicEventID::RespondTowerDefenseInfo:
        return "MHLogicEventID::RespondTowerDefenseInfo";
    case MHLogicEventID::SoulStone:
        return "MHLogicEventID::SoulStone";
    case MHLogicEventID::SoulStoneStageUP:
        return "MHLogicEventID::SoulStoneStageUP";
    case MHLogicEventID::SoulStoneAttrUP:
        return "MHLogicEventID::SoulStoneAttrUP";
    case MHLogicEventID::SoulStoneReset:
        return "MHLogicEventID::SoulStoneReset";
    case MHLogicEventID::SoulStoneUnlock:
        return "MHLogicEventID::SoulStoneUnlock";
    case MHLogicEventID::SoulStoneLevelUp:
        return "MHLogicEventID::SoulStoneLevelUp";
    case MHLogicEventID::EquipQuickLevelUpOk:
        return "MHLogicEventID::EquipQuickLevelUpOk";
    case MHLogicEventID::RefreshGuardStoneView:
        return "MHLogicEventID::RefreshGuardStoneView";
    case MHLogicEventID::AbnormalState_Restricted_QTE_UI_Show:
        return "MHLogicEventID::AbnormalState_Restricted_QTE_UI_Show";
    case MHLogicEventID::AbnormalState_Restricted_QTE_UI_Notify:
        return "MHLogicEventID::AbnormalState_Restricted_QTE_UI_Notify";
    case MHLogicEventID::PetMonsterPlayerDeadNotify:
        return "MHLogicEventID::PetMonsterPlayerDeadNotify";
    case MHLogicEventID::UpdateGuildActivityGridNotify:
        return "MHLogicEventID::UpdateGuildActivityGridNotify";
    case MHLogicEventID::NewbieWeaponTutorial:
        return "MHLogicEventID::NewbieWeaponTutorial";
    case MHLogicEventID::OnCloseWeaponTutorialEnd:
        return "MHLogicEventID::OnCloseWeaponTutorialEnd";
    case MHLogicEventID::UpdateGuildActInfoNotify:
        return "MHLogicEventID::UpdateGuildActInfoNotify";
    case MHLogicEventID::UltimateChallengeSettlementData:
        return "MHLogicEventID::UltimateChallengeSettlementData";
    case MHLogicEventID::UltimateChallengeSettlement:
        return "MHLogicEventID::UltimateChallengeSettlement";
    case MHLogicEventID::UltimateChallengeRankNtf:
        return "MHLogicEventID::UltimateChallengeRankNtf";
    case MHLogicEventID::XmasActivityRefresh:
        return "MHLogicEventID::XmasActivityRefresh";
    case MHLogicEventID::UpdateGuildTaskDetailInfo:
        return "MHLogicEventID::UpdateGuildTaskDetailInfo";
    case MHLogicEventID::UpdateSoulStonePoint:
        return "MHLogicEventID::UpdateSoulStonePoint";
    case MHLogicEventID::MVPAngrySkillNotify:
        return "MHLogicEventID::MVPAngrySkillNotify";
    case MHLogicEventID::RefreshGuilderActivityInfo:
        return "MHLogicEventID::RefreshGuilderActivityInfo";
    case MHLogicEventID::PlayVideo:
        return "MHLogicEventID::PlayVideo";
    case MHLogicEventID::UpdateProfoundData:
        return "MHLogicEventID::UpdateProfoundData";
    case MHLogicEventID::ReqGuildInfoData:
        return "MHLogicEventID::ReqGuildInfoData";
    case MHLogicEventID::RewardSucced:
        return "MHLogicEventID::RewardSucced";
    case MHLogicEventID::SetGameMaxFps:
        return "MHLogicEventID::SetGameMaxFps";
    case MHLogicEventID::StarHuntLevelMode:
        return "MHLogicEventID::StarHuntLevelMode";
    case MHLogicEventID::InstanceSelect_responseServerUnlockStar:
        return "MHLogicEventID::InstanceSelect_responseServerUnlockStar";
    case MHLogicEventID::GlidingBombUI:
        return "MHLogicEventID::GlidingBombUI";
    case MHLogicEventID::GlidingBombCountsRefresh:
        return "MHLogicEventID::GlidingBombCountsRefresh";
    case MHLogicEventID::XmasActivityGetMeal:
        return "MHLogicEventID::XmasActivityGetMeal";
    case MHLogicEventID::ApplyForCommderResult:
        return "MHLogicEventID::ApplyForCommderResult";
    case MHLogicEventID::RequireCommderResult:
        return "MHLogicEventID::RequireCommderResult";
    case MHLogicEventID::ClearDamageNumber:
        return "MHLogicEventID::ClearDamageNumber";
    case MHLogicEventID::OnDivinationReset:
        return "MHLogicEventID::OnDivinationReset";
    case MHLogicEventID::StartCatCuisine:
        return "MHLogicEventID::StartCatCuisine";
    case MHLogicEventID::EndCatCuisine:
        return "MHLogicEventID::EndCatCuisine";
    case MHLogicEventID::QuitCommanderResult:
        return "MHLogicEventID::QuitCommanderResult";
    case MHLogicEventID::HideUI:
        return "MHLogicEventID::HideUI";
    case MHLogicEventID::OnDivinationRefresh:
        return "MHLogicEventID::OnDivinationRefresh";
    case MHLogicEventID::RequireUltimateChangeMonsterResult:
        return "MHLogicEventID::RequireUltimateChangeMonsterResult";
    case MHLogicEventID::ToggleVolFog:
        return "MHLogicEventID::ToggleVolFog";
    case MHLogicEventID::OnWeaponStyleRsp:
        return "MHLogicEventID::OnWeaponStyleRsp";
    case MHLogicEventID::OnSoulBeastRsp:
        return "MHLogicEventID::OnSoulBeastRsp";
    case MHLogicEventID::IdleViewWardrobeLeave:
        return "MHLogicEventID::IdleViewWardrobeLeave";
    case MHLogicEventID::SetFitAvatarListFinish:
        return "MHLogicEventID::SetFitAvatarListFinish";
    case MHLogicEventID::OnSuperHunterUIRefresh:
        return "MHLogicEventID::OnSuperHunterUIRefresh";
    case MHLogicEventID::OnLegendPearlUpgradeRsp:
        return "MHLogicEventID::OnLegendPearlUpgradeRsp";
    case MHLogicEventID::RefreshGuideBookUI:
        return "MHLogicEventID::RefreshGuideBookUI";
    case MHLogicEventID::MaxStaAndHPChangeNotify:
        return "MHLogicEventID::MaxStaAndHPChangeNotify";
    case MHLogicEventID::MaxStaAndHPChangeNotifyLevle:
        return "MHLogicEventID::MaxStaAndHPChangeNotifyLevle";
    case MHLogicEventID::OnLegendPearlDeriveRsp:
        return "MHLogicEventID::OnLegendPearlDeriveRsp";
    case MHLogicEventID::RefreshWorldCupRankList:
        return "MHLogicEventID::RefreshWorldCupRankList";
    case MHLogicEventID::RefreshHunterBattle:
        return "MHLogicEventID::RefreshHunterBattle";
    case MHLogicEventID::GetHunterBattleShipRequireDatas:
        return "MHLogicEventID::GetHunterBattleShipRequireDatas";
    case MHLogicEventID::SetHunterBattleRankDetails:
        return "MHLogicEventID::SetHunterBattleRankDetails";
    case MHLogicEventID::GetAllCommerce:
        return "MHLogicEventID::GetAllCommerce";
    case MHLogicEventID::GetSeaSandData:
        return "MHLogicEventID::GetSeaSandData";
    case MHLogicEventID::RefreshShipRequireData:
        return "MHLogicEventID::RefreshShipRequireData";
    case MHLogicEventID::RefreshAllGuildSailData:
        return "MHLogicEventID::RefreshAllGuildSailData";
    case MHLogicEventID::GetPlayerShipData:
        return "MHLogicEventID::GetPlayerShipData";
    case MHLogicEventID::RefreshGuildCommerceBuff:
        return "MHLogicEventID::RefreshGuildCommerceBuff";
    case MHLogicEventID::RefreshGuildShipByGuildID:
        return "MHLogicEventID::RefreshGuildShipByGuildID";
    case MHLogicEventID::OnEnterLevelClick:
        return "MHLogicEventID::OnEnterLevelClick";
    case MHLogicEventID::GuildFuncRecordRsp:
        return "MHLogicEventID::GuildFuncRecordRsp";
    case MHLogicEventID::RefreshWarshipTipData:
        return "MHLogicEventID::RefreshWarshipTipData";
    case MHLogicEventID::GuildInventoryRecordRsp:
        return "MHLogicEventID::GuildInventoryRecordRsp";
    case MHLogicEventID::RefreshGrabableTimes:
        return "MHLogicEventID::RefreshGrabableTimes";
    case MHLogicEventID::RefreshStartBoatTimes:
        return "MHLogicEventID::RefreshStartBoatTimes";
    case MHLogicEventID::SecretResearchRefreshCurTabData:
        return "MHLogicEventID::SecretResearchRefreshCurTabData";
    case MHLogicEventID::SecretResearchRefreshResearchData:
        return "MHLogicEventID::SecretResearchRefreshResearchData";
    case MHLogicEventID::IllustrateBookCollectCardRsp:
        return "MHLogicEventID::IllustrateBookCollectCardRsp";
    case MHLogicEventID::MonsterCardExchangePhyCardRsp:
        return "MHLogicEventID::MonsterCardExchangePhyCardRsp";
    case MHLogicEventID::MonsterCardGetRewardRsp:
        return "MHLogicEventID::MonsterCardGetRewardRsp";
    case MHLogicEventID::MonsterCardUpdateCardInfo:
        return "MHLogicEventID::MonsterCardUpdateCardInfo";
    case MHLogicEventID::DragonBoxShopRsp:
        return "MHLogicEventID::DragonBoxShopRsp";
    case MHLogicEventID::DragonBoxFreshNumOpenRsp:
        return "MHLogicEventID::DragonBoxFreshNumOpenRsp";
    case MHLogicEventID::IllustrateBookGoToMonster:
        return "MHLogicEventID::IllustrateBookGoToMonster";
    case MHLogicEventID::DailyConsumptionOpenActivityRsp:
        return "MHLogicEventID::DailyConsumptionOpenActivityRsp";
    case MHLogicEventID::DailyConsumptionGetRewardRsp:
        return "MHLogicEventID::DailyConsumptionGetRewardRsp";
    default:
        return "Unknown MHLogicEventID!";
    }
}

#endif //MHO_LAUNCHER_INTERNAL_NOTIFICATION_IDS