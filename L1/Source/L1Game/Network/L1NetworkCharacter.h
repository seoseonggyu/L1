#pragma once

#include "Character/LyraCharacter.h"


#include "L1NetworkCharacter.generated.h"

class ULyraAbilitySystemComponent;

// ���� Lyra�� Ability�� PlayerState���� �����ؼ� �����Ѵ�(Dedicated Server).
// ����, IOCP ������ ������ ���� ���� ĳ���͸� ���� Ability�� ��������

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