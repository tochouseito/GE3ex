#include "TitleScene.h"
#include"Input.h"
#include"GameScene.h"
#include"SceneManager.h"
#include"imgui.h"
void TitleScene::Initialize()
{
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Scene");
	if (ImGui::Button("ChangeScene")) {
		/*次のシーンを生成*/
		BaseScene* scene = new GameScene();
		/*シーン切り替え依頼*/
		SceneManager::GetInstance()->SetNextScene(scene);
	}
	ImGui::End();
#endif // _DEBUG
}

void TitleScene::Draw()
{
}

void TitleScene::ChangeScene()
{
	
}
