// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "L1GameplayTagStack.generated.h"

struct FL1GameplayTagStackContainer;
struct FNetDeltaSerializeInfo;

/**
 * Represents one stack of a gameplay tag (tag + count)
 */
USTRUCT(BlueprintType)
struct FL1GameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FL1GameplayTagStack()
	{}

	FL1GameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount) { }
public:
	const FGameplayTag& GetStackTag() const { return Tag; }
	int32 GetStackCount() const { return StackCount; }

	FString GetDebugString() const;

private:
	friend FL1GameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

/** Container of gameplay tag stacks */
USTRUCT(BlueprintType)
struct FL1GameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	FL1GameplayTagStackContainer()
	//	: Owner(nullptr)
	{
	}

public:
	void AddStack(FGameplayTag Tag, int32 StackCount);
	void RemoveStack(FGameplayTag Tag);

public:
	const TArray<FL1GameplayTagStack>& GetStacks() const { return Stacks; }

	int32 GetStackCount(FGameplayTag Tag) const { return TagToCountMap.FindRef(Tag); }

	// Returns true if there is at least one stack of the specified tag
	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FL1GameplayTagStack, FL1GameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FL1GameplayTagStack> Stacks;
	
	// Accelerated list of tag stacks for queries
	TMap<FGameplayTag, int32> TagToCountMap;
};

template<>
struct TStructOpsTypeTraits<FL1GameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FL1GameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};