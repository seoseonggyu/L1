
#include "L1NetworkPawnData.h"
#include "System/LyraAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1NetworkPawnData)

UL1NetworkPawnData::UL1NetworkPawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
}

const UL1NetworkPawnData& UL1NetworkPawnData::Get()
{
	return ULyraAssetManager::Get().GetNetworkPawnData();
}
