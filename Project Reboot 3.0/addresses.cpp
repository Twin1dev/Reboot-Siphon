#include "addresses.h"

#include "UObjectGlobals.h"
#include "World.h"
#include "NetDriver.h"
#include "GameSession.h"

#include "NetSerialization.h"

#include "Array.h"
#include "AbilitySystemComponent.h"

#include "finder.h"

void Addresses::SetupVersion()
{
	// if (false)
	{
		Engine_Version = 423;
		Fortnite_Version = 10.40;
	}

	if (false)
	{
		Engine_Version = 424;
		Fortnite_Version = 11.01;
	}

	if (false)
	{
		Engine_Version = 425;
		Fortnite_Version = 12.41;
	}

	FFastArraySerializer::bNewSerializer = Fortnite_Version >= 8.30;
}

void Addresses::FindAll()
{
	Addresses::ProcessEvent = FindProcessEvent();
	Addresses::StaticFindObject = FindStaticFindObject();
	Addresses::GetPlayerViewpoint = FindGetPlayerViewpoint();
	Addresses::CreateNetDriver = FindCreateNetDriver();
	Addresses::InitHost = FindInitHost();
	Addresses::PauseBeaconRequests = FindPauseBeaconRequests();
	Addresses::SpawnActor = FindSpawnActor();
	Addresses::InitListen = FindInitListen();
	Addresses::SetWorld = FindSetWorld();
	Addresses::KickPlayer = FindKickPlayer();
	Addresses::TickFlush = FindTickFlush();
	Addresses::GetNetMode = FindGetNetMode();
	Addresses::Realloc = FindRealloc();
	Addresses::CollectGarbage = FindCollectGarbage();
	Addresses::NoMCP = FindNoMCP();
	Addresses::PickTeam = FindPickTeam();
	Addresses::InternalTryActivateAbility = FindInternalTryActivateAbility();
	Addresses::GiveAbility = FindGiveAbility();
	Addresses::CantBuild = FindCantBuild();
	Addresses::ReplaceBuildingActor = FindReplaceBuildingActor();
}

void Addresses::Print()
{
	auto Base = __int64(GetModuleHandleW(0));

	LOG_INFO(LogDev, "Base: 0x{:x}", Base);
	LOG_INFO(LogDev, "ProcessEvent: 0x{:x}", ProcessEvent - Base);
	LOG_INFO(LogDev, "StaticFindObject: 0x{:x}", StaticFindObject - Base);
	LOG_INFO(LogDev, "GetPlayerViewpoint: 0x{:x}", GetPlayerViewpoint - Base);
	LOG_INFO(LogDev, "CreateNetDriver: 0x{:x}", CreateNetDriver - Base);
	LOG_INFO(LogDev, "InitHost: 0x{:x}", InitHost - Base);
	LOG_INFO(LogDev, "PauseBeaconRequests: 0x{:x}", PauseBeaconRequests - Base);
	LOG_INFO(LogDev, "SpawnActor: 0x{:x}", SpawnActor - Base);
	LOG_INFO(LogDev, "InitListen: 0x{:x}", InitListen - Base);
	LOG_INFO(LogDev, "SetWorld: 0x{:x}", SetWorld - Base);
	LOG_INFO(LogDev, "KickPlayer: 0x{:x}", KickPlayer - Base);
	LOG_INFO(LogDev, "TickFlush: 0x{:x}", TickFlush - Base);
	LOG_INFO(LogDev, "GetNetMode: 0x{:x}", GetNetMode - Base);
	LOG_INFO(LogDev, "Realloc: 0x{:x}", Realloc - Base);
	LOG_INFO(LogDev, "CollectGarbage: 0x{:x}", CollectGarbage - Base);
	LOG_INFO(LogDev, "NoMCP: 0x{:x}", NoMCP - Base);
	LOG_INFO(LogDev, "PickTeam: 0x{:x}", PickTeam - Base);
	LOG_INFO(LogDev, "InternalTryActivateAbility: 0x{:x}", InternalTryActivateAbility - Base);
	LOG_INFO(LogDev, "GiveAbility: 0x{:x}", GiveAbility - Base);
	LOG_INFO(LogDev, "CantBuild: 0x{:x}", CantBuild - Base);
	LOG_INFO(LogDev, "ReplaceBuildingActor: 0x{:x}", ReplaceBuildingActor - Base);
}

void Offsets::FindAll()
{
	Offsets::Offset_Internal = Engine_Version >= 425 && std::floor(Fortnite_Version) < 20 ? 0x4C : 0x44;
	Offsets::SuperStruct = Engine_Version >= 422 ? 0x40 : 0x30;
	Offsets::Children = Engine_Version >= 425 ? 0x50 : Offsets::SuperStruct + 8;
	Offsets::PropertiesSize = Offsets::Children + 8;

	if (Engine_Version == 420 || Engine_Version == 421)
		Offsets::Func = 0xB0;
	else if (Engine_Version == 423 || Engine_Version == 424)
		Offsets::Func = 0xC0;
	else if (Engine_Version == 425)
		Offsets::Func = 0xF0;
	else if (Engine_Version >= 426)
		Offsets::Func = 0xD8;

	if (Engine_Version == 420)
		Offsets::ServerReplicateActors = 0x53;
	else if (Engine_Version == 421)
		Offsets::ServerReplicateActors = std::floor(Fortnite_Version) == 5 ? 0x54 : 0x56;
	else if (Engine_Version >= 422 && Engine_Version <= 424)
		Offsets::ServerReplicateActors = Fortnite_Version >= 7.40 && Fortnite_Version < 8.40 ? 0x57 :
		Engine_Version == 424 ? (Fortnite_Version >= 11.00 && Fortnite_Version <= 11.01 ? 0x57 :
			(Fortnite_Version == 11.30 || Fortnite_Version == 11.31 ? 0x59 : 0x5A)) : 0x56;

	// ^ I know this makes no sense, 7.40-8.40 is 0x57, other 7-10 is 0x56, 11.00-11.01 = 0x57, 11.31 = 0x59, other S11 is 0x5A

	else if (std::floor(Fortnite_Version) == 12 || std::floor(Fortnite_Version) == 13)
		Offsets::ServerReplicateActors = 0x5D;
	else if (std::floor(Fortnite_Version) == 14 || Fortnite_Version <= 15.2) // never tested 15.2
		Offsets::ServerReplicateActors = 0x5E;
	else if (Fortnite_Version >= 15.3 && Engine_Version < 500) // 15.3-18 = 0x5F
		Offsets::ServerReplicateActors = 0x5F;
	else if (std::floor(Fortnite_Version) >= 19 && std::floor(Fortnite_Version) <= 20)
		Offsets::ServerReplicateActors = 0x66;
	else if (std::floor(Fortnite_Version) >= 21)
		Offsets::ServerReplicateActors = 0x67; // checked onb 22.30
}

void Offsets::Print()
{
	LOG_INFO(LogDev, "Offset_Internal: 0x{:x}", Offset_Internal);
	LOG_INFO(LogDev, "SuperStruct: 0x{:x}", SuperStruct);
	LOG_INFO(LogDev, "Children: 0x{:x}", Children);
	LOG_INFO(LogDev, "PropertiesSize: 0x{:x}", PropertiesSize);
	LOG_INFO(LogDev, "Func: 0x{:x}", Func);
}

void Addresses::Init()
{
	UObject::ProcessEventOriginal = decltype(UObject::ProcessEventOriginal)(ProcessEvent);
	StaticFindObjectOriginal = decltype(StaticFindObjectOriginal)(StaticFindObject);
	UWorld::SpawnActorOriginal = decltype(UWorld::SpawnActorOriginal)(SpawnActor);
	UNetDriver::InitListenOriginal = decltype(UNetDriver::InitListenOriginal)(InitListen);
	UNetDriver::SetWorldOriginal = decltype(UNetDriver::SetWorldOriginal)(SetWorld);
	AGameSession::KickPlayerOriginal = decltype(AGameSession::KickPlayerOriginal)(KickPlayer);
	UNetDriver::TickFlushOriginal = decltype(UNetDriver::TickFlushOriginal)(TickFlush);
	FMemory::Realloc = decltype(FMemory::Realloc)(Realloc);
	UAbilitySystemComponent::GiveAbilityOriginal = decltype(UAbilitySystemComponent::GiveAbilityOriginal)(GiveAbility);
	UAbilitySystemComponent::InternalTryActivateAbilityOriginal = decltype(UAbilitySystemComponent::InternalTryActivateAbilityOriginal)(InternalTryActivateAbility);

	// if (Engine_Version >= 421) ChunkedObjects = decltype(ChunkedObjects)(ObjectArray);
	// else UnchunkedObjects = decltype(UnchunkedObjects)(ObjectArray);
}