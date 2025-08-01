// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/L1CameraMode_TopDown.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(L1CameraMode_TopDown)

UL1CameraMode_TopDown::UL1CameraMode_TopDown()
{
}

void UL1CameraMode_TopDown::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	//FRotator PivotRotation = GetPivotRotation();

	//PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	//PivotRotation.Pitch += 50.f;
	
	PivotLocation.Z += 1000.f;
	PivotLocation.X -= 500.f;


	FRotator PivotRotation;
	PivotRotation.Pitch = -60.f; // ������ �Ʒ��� 60�� �����ٺ���
	PivotRotation.Yaw = 0.f;     // �ʿ信 ���� ȸ�� (ĳ���� ���� ����, ���� ��)
	PivotRotation.Roll = 0.f;

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;
}
