#include <Windows.h>
#include <iostream>

#include "FortGameModeAthena.h"
#include "reboot.h"
#include "finder.h"
#include "hooking.h"
#include "GameSession.h"
#include "FortPlayerControllerAthena.h"
#include "AbilitySystemComponent.h"

enum ENetMode
{
    NM_Standalone,
    NM_DedicatedServer,
    NM_ListenServer,
    NM_Client,
    NM_MAX,
};

static ENetMode GetNetModeHook() { /* std::cout << "AA!\n"; */ return ENetMode::NM_DedicatedServer; }
static void NoMCPHook() { return; }
static void CollectGarbageHook() { return; }

DWORD WINAPI Main(LPVOID)
{
    InitLogger();

    std::cin.tie(0);
    std::cout.tie(0);
    std::ios_base::sync_with_stdio(false);

    auto MH_InitCode = MH_Initialize();

    if (MH_InitCode != MH_OK)
    {
        LOG_ERROR(LogInit, "Failed to initialize MinHook {}!", MH_StatusToString(MH_InitCode));
        return 1;
    }

    LOG_INFO(LogInit, "Initializing Project Reboot!");

    Addresses::SetupVersion();

    Offsets::FindAll(); // We have to do this before because FindCantBuild uses FortAIController.CreateBuildingActor
    Offsets::Print();

    Addresses::FindAll();
    Addresses::Print();
    Addresses::Init();

    static auto GameModeDefault = FindObject<UClass>(L"/Script/FortniteGame.Default__FortGameModeAthena");
    static auto FortPlayerControllerAthenaDefault = FindObject<UClass>(L"/Script/FortniteGame.Default__FortPlayerControllerAthena");
    static auto FortAbilitySystemComponentAthenaDefault = FindObject<UClass>(L"/Script/FortniteGame.Default__FortAbilitySystemComponentAthena");

    static auto SwitchLevel = FindObject<UFunction>(L"/Script/Engine.PlayerController.SwitchLevel");
    FString Level = Engine_Version < 424 ? L"Athena_Terrain" : Engine_Version >= 500 ? Engine_Version >= 501 ? L"Asteria_Terrain" : L"Artemis_Terrain" : L"Apollo_Terrain";

    // if (Hooking::MinHook::Hook((PVOID)Addresses::NoMCP, (PVOID)NoMCPHook, nullptr))
    {
        LOG_INFO(LogHook, "Hooking GetNetMode!");
        Hooking::MinHook::Hook((PVOID)Addresses::GetNetMode, (PVOID)GetNetModeHook, nullptr);
    }

    GetLocalPlayerController()->ProcessEvent(SwitchLevel, &Level);

    auto& LocalPlayers = GetLocalPlayers();

    if (LocalPlayers.Num() && LocalPlayers.Data)
    {
        LocalPlayers.Remove(0);
    }

    Hooking::MinHook::Hook(GameModeDefault, FindObject<UFunction>(L"/Script/Engine.GameMode.ReadyToStartMatch"), AFortGameModeAthena::Athena_ReadyToStartMatchHook, 
        (PVOID*)&AFortGameModeAthena::Athena_ReadyToStartMatchOriginal, false);
    Hooking::MinHook::Hook(GameModeDefault, FindObject<UFunction>(L"/Script/Engine.GameModeBase.SpawnDefaultPawnFor"),
        AGameModeBase::SpawnDefaultPawnForHook, nullptr, false);
    Hooking::MinHook::Hook(GameModeDefault, FindObject<UFunction>(L"/Script/Engine.GameModeBase.HandleStartingNewPlayer"), AFortGameModeAthena::Athena_HandleStartingNewPlayerHook,
        (PVOID*)&AFortGameModeAthena::Athena_HandleStartingNewPlayerOriginal, false);

    Hooking::MinHook::Hook(FortPlayerControllerAthenaDefault, FindObject<UFunction>(L"/Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem"), 
        AFortPlayerController::ServerExecuteInventoryItemHook, nullptr, false);
    Hooking::MinHook::Hook(FortPlayerControllerAthenaDefault, FindObject<UFunction>(L"/Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor"), 
        AFortPlayerController::ServerCreateBuildingActorHook, nullptr, false);
    Hooking::MinHook::Hook(FortPlayerControllerAthenaDefault, FindObject<UFunction>("/Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor"),
        AFortPlayerController::ServerBeginEditingBuildingActorHook, nullptr, false);
    Hooking::MinHook::Hook(FortPlayerControllerAthenaDefault, FindObject<UFunction>("/Script/FortniteGame.FortPlayerController.ServerEditBuildingActor"),
        AFortPlayerController::ServerEditBuildingActorHook, nullptr, false);
    Hooking::MinHook::Hook(FortPlayerControllerAthenaDefault, FindObject<UFunction>("/Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor"),
        AFortPlayerController::ServerEndEditingBuildingActorHook, nullptr, false);
    Hooking::MinHook::Hook(FortPlayerControllerAthenaDefault, FindObject<UFunction>(L"/Script/Engine.PlayerController.ServerAcknowledgePossession"),
        AFortPlayerControllerAthena::ServerAcknowledgePossessionHook, nullptr, false);

    Hooking::MinHook::Hook(FortAbilitySystemComponentAthenaDefault, FindObject<UFunction>(L"/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility"),
        UAbilitySystemComponent::ServerTryActivateAbilityHook, nullptr, false);
    Hooking::MinHook::Hook(FortAbilitySystemComponentAthenaDefault, FindObject<UFunction>(L"/Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbilityWithEventData"),
        UAbilitySystemComponent::ServerTryActivateAbilityWithEventDataHook, nullptr, false);
    Hooking::MinHook::Hook(FortAbilitySystemComponentAthenaDefault, FindObject<UFunction>(L"/Script/GameplayAbilities.AbilitySystemComponent.ServerAbilityRPCBatch"),
        UAbilitySystemComponent::ServerAbilityRPCBatchHook, nullptr, false);

    if (Engine_Version >= 424)
    {
        static auto FortControllerComponent_AircraftDefault = FindObject<UClass>(L"/Script/FortniteGame.Default__FortControllerComponent_Aircraft");

        Hooking::MinHook::Hook(FortControllerComponent_AircraftDefault, FindObject<UFunction>(L"/Script/FortniteGame.FortControllerComponent_Aircraft.ServerAttemptAircraftJump"),
            AFortPlayerController::ServerAttemptAircraftJumpHook, nullptr, false);
    }

    Hooking::MinHook::Hook((PVOID)Addresses::KickPlayer, (PVOID)AGameSession::KickPlayerHook, (PVOID*)&AGameSession::KickPlayerOriginal);
    Hooking::MinHook::Hook((PVOID)Addresses::TickFlush, (PVOID)UNetDriver::TickFlushHook, (PVOID*)&UNetDriver::TickFlushOriginal);

    Hooking::MinHook::Hook((PVOID)Addresses::CollectGarbage, (PVOID)CollectGarbageHook, nullptr);
    Hooking::MinHook::Hook((PVOID)Addresses::PickTeam, (PVOID)AFortGameModeAthena::Athena_PickTeamHook, nullptr);

    srand(time(0));

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, 0, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

