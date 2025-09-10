#include "L1AbilityTask_WaitForTick.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1AbilityTask_WaitForTick)

UL1AbilityTask_WaitForTick::UL1AbilityTask_WaitForTick(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

UL1AbilityTask_WaitForTick* UL1AbilityTask_WaitForTick::WaitForTick(UGameplayAbility* OwningAbility)
{
	UL1AbilityTask_WaitForTick* Task = NewAbilityTask<UL1AbilityTask_WaitForTick>(OwningAbility);
	return Task;
}

void UL1AbilityTask_WaitForTick::Activate()
{
	Super::Activate();
}

void UL1AbilityTask_WaitForTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTick.Broadcast(DeltaTime);
	}
}
