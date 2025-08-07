#include "L1PocketStage.h"

#include "L1PocketWorldSubsystem.h"
#include "PocketCapture.h"
#include "PocketCaptureSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/LightComponent.h"
#include "GameFramework/Character.h"
#include "Item/Managers/L1CosmeticManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1PocketStage)

AL1PocketStage::AL1PocketStage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(RootSceneComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocationAndRotation(FVector(425.27f, 0.f, 65.f), FRotator(-10.f, -180.f, 0.f));
	CameraComponent->FieldOfView = 22.f;
	CameraComponent->AspectRatio = 0.74f;
	CameraComponent->SetConstraintAspectRatio(true);
	
	CharacterSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CharacterSpawnPoint"));
	CharacterSpawnPoint->SetupAttachment(GetRootComponent());
}

void AL1PocketStage::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Owner = this;
	
	SpawnedCharacter = GetWorld()->SpawnActor<ACharacter>(CharacterClass, CharacterSpawnPoint->GetComponentTransform(), SpawnParameters);
	SpawnedCharacter->AttachToComponent(CharacterSpawnPoint, FAttachmentTransformRules::KeepWorldTransform);

	if (UL1PocketWorldSubsystem* PocketWorldSubsystem = GetWorld()->GetSubsystem<UL1PocketWorldSubsystem>())
	{
		PocketWorldSubsystem->SetPocketStage(this);
	}
	
	if (UPocketCaptureSubsystem* PocketCaptureSubsystem = GetWorld()->GetSubsystem<UPocketCaptureSubsystem>())
	{
		PocketCapture = PocketCaptureSubsystem->CreateThumbnailRenderer(PocketCaptureClass);
		PocketCapture->SetCaptureTarget(this);

		if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
		{
			FViewport* Viewport = GEngine->GameViewport->Viewport;
			RefreshRenderTargetSize(Viewport, 0);
			ViewportResizedDelegateHandle = Viewport->ViewportResizedEvent.AddUObject(this, &AL1PocketStage::RefreshRenderTargetSize);
		}
	
		RefreshLightingChannelToActors();
	}
}

void AL1PocketStage::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PocketCapture)
	{
		if (UPocketCaptureSubsystem* PocketCaptureSubsystem = GetWorld()->GetSubsystem<UPocketCaptureSubsystem>())
		{
			PocketCaptureSubsystem->DestroyThumbnailRenderer(PocketCapture);
		}
		
		PocketCapture->ReleaseResources();
		PocketCapture = nullptr;
	}

	if (SpawnedCharacter)
	{
		SpawnedCharacter->Destroy();
		SpawnedCharacter = nullptr;
	}

	if (ViewportResizedDelegateHandle.IsValid())
	{
		if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
		{
			FViewport* Viewport = GEngine->GameViewport->Viewport;
			Viewport->ViewportResizedEvent.Remove(ViewportResizedDelegateHandle);
			ViewportResizedDelegateHandle.Reset();
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void AL1PocketStage::RefreshLightingChannelToActors()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors, true, true);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (AttachedActor)
		{
			TArray<UPrimitiveComponent*> PrimitiveComponents;
			AttachedActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
				
			for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
			{
				if (PrimitiveComponent)
				{
					PrimitiveComponent->SetLightingChannels(false, true, false);
				}
			}

			TArray<ULightComponent*> LightComponents;
			AttachedActor->GetComponents<ULightComponent>(LightComponents);

			for (ULightComponent* LightComponent : LightComponents)
			{
				if (LightComponent)
				{
					LightComponent->SetLightingChannels(false, true, false);
				}
			}
		}
	}
}

void AL1PocketStage::RefreshRenderTargetSize(FViewport* InViewport, uint32 InValue)
{
	FVector2D RenderTargetSize;
	FVector2D ViewportSize = FVector2D(1920, 1080);

	if (InViewport)
	{
		ViewportSize = FVector2D(InViewport->GetSizeXY());
	}

	ViewportSize /= 2.f;
	
	const float ViewportAspectRatio = ViewportSize.X / ViewportSize.Y;
	const float CameraAspectRatio = CameraComponent->AspectRatio;
	
	if (ViewportAspectRatio < CameraAspectRatio)
	{
		RenderTargetSize = FVector2D(ViewportSize.X, ViewportSize.X * (1.f / CameraAspectRatio));
	}
	else
	{
		RenderTargetSize = FVector2D(ViewportSize.Y / (1.f / CameraAspectRatio), ViewportSize.Y);
	}

	if (PocketCapture)
	{
		PocketCapture->SetRenderTargetSize(RenderTargetSize.X, RenderTargetSize.Y);
	}
}

UL1CosmeticManagerComponent* AL1PocketStage::GetCosmeticManager() const
{
	UL1CosmeticManagerComponent* CosmeticManager = nullptr;
	if (SpawnedCharacter)
	{
		CosmeticManager = SpawnedCharacter->GetComponentByClass<UL1CosmeticManagerComponent>();
	}
	return CosmeticManager;
}
