#pragma once

#include "Character/LyraCharacter.h"


#include "L1NetworkCharacter.generated.h"

class ULyraAbilitySystemComponent;

// 기존 Lyra는 Ability를 PlayerState에서 생성해서 관리한다(Dedicated Server).
// 따라서, IOCP 서버와 연동할 때는 따로 캐릭터를 만들어서 Ability를 관리하자

UCLASS()
class L1GAME_API AL1NetworkCharacter : public ALyraCharacter
{
	GENERATED_BODY()

public:
	AL1NetworkCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	virtual ULyraAbilitySystemComponent* GetLyraAbilitySystemComponent() const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(VisibleAnywhere, Category = "L1|NetworkCharacter")
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;
};