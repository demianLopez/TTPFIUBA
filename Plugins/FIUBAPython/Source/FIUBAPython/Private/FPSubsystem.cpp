// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSubsystem.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

UFPSubsystem* UFPSubsystem::Get()
{
	return GEngine->GetEngineSubsystem<UFPSubsystem>();
}

void UFPSubsystem::GetResponse()
{
	FHttpModule* Http = &FHttpModule::Get();

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL("http://localhost:8000/evaluation");
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	
	TSharedPtr<FJsonObject> JsonObject = GetBody();

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	
	Request->SetContentAsString(JsonString);

	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid())
		{
			FString ResponseString = Response->GetContentAsString();
			UE_LOG(LogTemp, Log, TEXT("Respuesta del Servidor: %s"), *ResponseString);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error en la solicitud HTTP"));
		}
	});
	
	Request->ProcessRequest();
}

TSharedPtr<FJsonObject> UFPSubsystem::GetBody()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("title", "titleex");
	JsonObject->SetStringField("body", "content");
	JsonObject->SetNumberField("userId", 1);

	return JsonObject;
}
