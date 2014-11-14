#include <DirectXMath.h>
#include <objbase.h>
#include "Window.h"
#include "./dx11/DX11ThinWrapper.h"
#include "./dx11/DX11GlobalDevice.h"

#include "./dx11/DX11Drawable.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE,
	LPTSTR,
	INT
) {
	::CoInitialize(NULL); // �Ȃ��Ă��������ǏI������O��f��
	
	// �E�B���h�E����
	auto modeDesc = dx11::gi::display::GetOptDisplayMode(800, 600);
	WindowGuard window(_T("Direct3D11"), hInstance, modeDesc.Width, modeDesc.Height);
	MSG msg = { 0 };

	{
		// ������
		auto guard = dx11::DX11DeviceSharedGuard();
		auto device = dx11::AccessDX11Device();

		auto context = dx11::d3::AccessD3Context(device);
		auto swapChain = dx11::d3::CreateSwapChain(&modeDesc, window.hWnd, device, true);

		dx11::d3::context::SetDefaultRenderTarget(swapChain.get());
		dx11::d3::context::SetDefaultViewport(swapChain.get());

		// VP�p �萔�o�b�t�@
		struct CameraParam {
			DirectX::XMFLOAT4X4	mtxView;
			DirectX::XMFLOAT4X4	mtxProj;
		};
		auto cameraBuffer = dx11::d3::buffer::CreateConstantBuffer(
			device, nullptr, sizeof(CameraParam), D3D11_CPU_ACCESS_WRITE
		);
	
		// �^�[�Q�b�g�r���[
		ID3D11RenderTargetView * rv;
		ID3D11DepthStencilView * dv;
		context->OMGetRenderTargets(1, &rv, &dv);
	
		// �T���v�����f���ǂݍ���
		auto model = dx11::Drawable("untitled.fbx", device);
		model.setScale(8);

		// �E�B���h�E�\��
		::ShowWindow(window.hWnd, SW_SHOW);
		::UpdateWindow(window.hWnd);

		// ���C�����[�v
		do {
			if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			} else {
				// �o�b�N�o�b�t�@���N���A����B
				static float ClearColor[4] = { 0.3f, 0.3f, 1.0f, 1.0f };
				context->ClearRenderTargetView(rv, ClearColor);

				// �[�x�o�b�t�@���N���A����B 
				context->ClearDepthStencilView(dv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

				// �J������VP�ݒ�
				dx11::mapping(cameraBuffer.get(), context.get(), [&](D3D11_MAPPED_SUBRESOURCE resource) {
					auto param = (CameraParam*)(resource.pData);
					auto aspectRatio = modeDesc.Width / (float)modeDesc.Height;
					auto mtxProj = DirectX::XMMatrixPerspectiveFovLH(3.1415926f / 2.0f, aspectRatio, 1.0f, 1000.0f);
					XMStoreFloat4x4(&param->mtxProj, DirectX::XMMatrixTranspose(mtxProj));
					auto mtxView = DirectX::XMMatrixLookAtLH(
						DirectX::XMVectorSet(3.0f, 18.0f, 108.0f, 1),
						DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1),
						DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1)
					);
					XMStoreFloat4x4(&param->mtxView, DirectX::XMMatrixTranspose(mtxView));
				});
				ID3D11Buffer * cameraBuffers[] = { cameraBuffer.get() };
				context->VSSetConstantBuffers(0, 1, cameraBuffers);

				// ���f���̕`��
				model.draw(context.get());

				if (FAILED(swapChain->Present(0, 0))) break;
			}
		} while (msg.message != WM_QUIT);
	}

	::CoUninitialize(); // �Ȃ��Ă��������ǏI������O��f��
	return msg.wParam;
}