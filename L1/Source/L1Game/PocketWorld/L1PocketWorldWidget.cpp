#include "L1PocketWorldWidget.h"

#include "L1PocketStage.h"
#include "L1PocketWorldSubsystem.h"
#include "PocketCapture.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1PocketWorldWidget)

UL1PocketWorldWidget::UL1PocketWorldWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    
}

void UL1PocketWorldWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UL1PocketWorldSubsystem* PocketWorldSubsystem = GetWorld()->GetSubsystem<UL1PocketWorldSubsystem>())
	{
		PocketWorldSubsystem->RegisterAndCallForGetPocketStage(GetOwningLocalPlayer(), FGetPocketStageDelegate::CreateUObject(this, &UL1PocketWorldWidget::OnPocketStageReady));
	}
}

void UL1PocketWorldWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshRenderTarget();
}

void UL1PocketWorldWidget::NativeDestruct()
{
	if (UMaterialInstanceDynamic* MaterialInstanceDynamic = Image_Render->GetDynamicMaterial())
	{
		MaterialInstanceDynamic->ClearParameterValues();
	}
	
	Super::NativeDestruct();
}

void UL1PocketWorldWidget::RefreshRenderTarget()
{
	if (CachedPocketStage.IsValid())
	{
		if (UPocketCapture* PocketCapture = CachedPocketStage->GetPocketCapture())
		{
			PocketCapture->CaptureDiffuse();
		}
	}
}

void UL1PocketWorldWidget::OnPocketStageReady(AL1PocketStage* PocketStage)
{
	CachedPocketStage = PocketStage;

	if (UPocketCapture* PocketCapture = CachedPocketStage->GetPocketCapture())
	{
		UTextureRenderTarget2D* DiffuseRenderTarget = PocketCapture->GetOrCreateDiffuseRenderTarget();
		
		if (UMaterialInstanceDynamic* MaterialInstanceDynamic = Image_Render->GetDynamicMaterial())
		{
			MaterialInstanceDynamic->SetTextureParameterValue("Diffuse", DiffuseRenderTarget);
		}
	}
}
