
#pragma once

#include "UObject/Interface.h"
#include "L1HighlightInterface.generated.h"

UINTERFACE(MinimalAPI)
class UL1HighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class L1GAME_API IL1HighlightInterface
{
	GENERATED_BODY()
public:

	virtual void Highlight() = 0;
	virtual void UnHighlight() = 0;
};
