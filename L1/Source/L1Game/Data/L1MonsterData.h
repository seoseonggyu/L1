#pragma once

#include "L1Define.h"
#include "L1MonsterData.generated.h"

class ULyraPawnData;

UCLASS(BlueprintType, Const, meta=(DisplayName="L1 Monster Data"))
class UL1MonsterData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	static const UL1MonsterData& Get();

public:
	ULyraPawnData* GetPawnData(EMonsterType MonsterType) const;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<EMonsterType, TObjectPtr<ULyraPawnData>> PawnDataMap;
};