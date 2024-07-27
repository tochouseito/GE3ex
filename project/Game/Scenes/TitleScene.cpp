#include "TitleScene.h"
#include"Input.h"
#include"GameScene.h"
#include"SceneManager.h"
void TitleScene::Initialize()
{
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	if (Input::GetInstance()->PushKey(DIK_RETURN)) {
		/*次のシーンを生成*/
		BaseScene* scene = new GameScene();
		/*シーン切り替え依頼*/
		
	}
}

void TitleScene::Draw()
{
}
