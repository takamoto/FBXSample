#pragma once
#include <memory>
#include <d3d11.h>
#include <d2d1_1.h>
#include <d3dcompiler.h>
#include <wincodec.h>
#include <wincodecsdk.h>
#include <functional>

namespace wic{
	std::shared_ptr<IWICImagingFactory> CreateWicFactory();
	std::shared_ptr<IWICBitmapDecoder> CreateDecoder(const TCHAR * path);
	std::shared_ptr<IWICBitmapFrameDecode> AccessFrame(IWICBitmapDecoder * dec);
	std::shared_ptr<IWICFormatConverter> CreateConverter();
};

namespace dx11 {
	namespace gi{
		namespace display{
			std::shared_ptr<IDXGIOutput> AccessDisplay(UINT i);
			void GetDisplayModes(DXGI_MODE_DESC* pModeDesc, UINT* pNum);
			DXGI_MODE_DESC GetOptDisplayMode(int width, int height);
		};

		std::shared_ptr<IDXGIFactory> AccessGIFactory(IDXGIAdapter * adapter);
		std::shared_ptr<IDXGIFactory> AccessGIFactory(ID3D11Device * device);

		std::shared_ptr<IDXGIDevice1> AccessInterface(ID3D11Device * device);
		
		std::shared_ptr<IDXGIAdapter> AccessAdapter(ID3D11Device * device);

		std::shared_ptr<IDXGISurface> AccessSurface(ID3D11Texture2D * tex);
	};
	namespace d2{
		namespace init{
			std::shared_ptr<ID2D1Device> CreateD2Device(ID3D11Device* device);
			std::shared_ptr<ID2D1HwndRenderTarget> CreateRenderTarget(HWND hWnd);
		};

		namespace drawer{
			std::shared_ptr<ID2D1SolidColorBrush> CreateBrush(D2D1::ColorF color, ID2D1RenderTarget * renderTarget);
			std::shared_ptr<ID2D1BitmapBrush> CreateBitmapBrush(ID2D1Bitmap * bitmap, ID2D1RenderTarget * target);
			std::shared_ptr<ID2D1BitmapBrush> CreateBitmapBrush(const TCHAR * path, ID2D1RenderTarget * target);
		};

		std::shared_ptr<ID2D1Factory1> AccessD2Factory();
		std::shared_ptr<ID2D1DeviceContext> AccessD2Context(ID2D1Device * device2d);
		std::shared_ptr<ID2D1Bitmap> AccessBitmap(ID3D11Texture2D* tex, ID2D1DeviceContext* deviceContext2d);
		std::shared_ptr<ID2D1Bitmap> CreateBitmap(const TCHAR * path, ID2D1RenderTarget * target);
	};
	namespace d3{
		namespace init{
			std::shared_ptr<ID3D11Device> CreateD3Device();
		};

		namespace context{
			void SetDefaultRenderTarget(IDXGISwapChain * swapChain);
			void SetDefaultViewport(IDXGISwapChain * swapChain);
			void SetDefaultRasterize(IDXGISwapChain * swapChain);
		};

		namespace shader{
			namespace detail{
				template<typename S> std::shared_ptr<S> CreateShader(
					ID3D11Device* device, BYTE* shader, size_t size,
					HRESULT(__stdcall ID3D11Device::*func)(const void *, SIZE_T, ID3D11ClassLinkage *, S **)
				);

				std::shared_ptr<ID3D11PixelShader> CreatePixelShader(ID3D11Device* device, BYTE* shader, size_t size);
				std::shared_ptr<ID3D11VertexShader> CreateVertexShader(ID3D11Device* device, BYTE* shader, size_t size);
				std::shared_ptr<ID3D11GeometryShader> CreateGeometryShader(ID3D11Device* device, BYTE* shader, size_t size);
			}
			std::shared_ptr<ID3DBlob> CompileShader(TCHAR source[], CHAR entryPoint[], CHAR target[]);
			std::shared_ptr<ID3D11PixelShader> CreatePixelShader(ID3D11Device* device, ID3DBlob * blob);
			std::shared_ptr<ID3D11VertexShader> CreateVertexShader(ID3D11Device* device, ID3DBlob * blob);
			std::shared_ptr<ID3D11GeometryShader> CreateGeometryShader(ID3D11Device* device, ID3DBlob * blob);
			std::shared_ptr<ID3D11InputLayout> CreateInputLayout(
				ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numOfLayout, ID3DBlob * blob
			);
		}

		namespace buffer{
			namespace detail{
				std::shared_ptr<ID3D11Buffer> CreateBuffer(
					ID3D11Device* device, void* data, size_t size, UINT cpuAccessFlag, D3D11_BIND_FLAG BindFlag
				);
			}

			inline std::shared_ptr<ID3D11Buffer> CreateVertexBuffer(
				ID3D11Device* device, void* data, size_t size, UINT cpuAccessFlag = 0
			) {
				return detail::CreateBuffer(device, data, size, cpuAccessFlag, D3D11_BIND_VERTEX_BUFFER);
			}

			inline std::shared_ptr<ID3D11Buffer> CreateIndexBuffer(
				ID3D11Device* device, void* data, size_t size, UINT cpuAccessFlag = 0
			) {
				return detail::CreateBuffer(device, data, size, cpuAccessFlag, D3D11_BIND_INDEX_BUFFER);
			}

			inline std::shared_ptr<ID3D11Buffer> CreateConstantBuffer(
				ID3D11Device* device, void* data, size_t size, UINT cpuAccessFlag = 0
			) {
				return detail::CreateBuffer(device, data, size, cpuAccessFlag, D3D11_BIND_CONSTANT_BUFFER);
			}
		};

		namespace detail{
			namespace depth{
				std::shared_ptr<ID3D11Texture2D> CreateDepthBuffer(ID3D11Device * device, D3D11_TEXTURE2D_DESC descDS);
				D3D11_TEXTURE2D_DESC CreateDepthBufferDESC(IDXGISwapChain * swapChain);
				DXGI_FORMAT TextureFormat2DepthStencilViewFormat(DXGI_FORMAT format);
			};
			std::shared_ptr<ID3D11RenderTargetView> CreateRenderTargetView(IDXGISwapChain * swapChain);
			std::shared_ptr<ID3D11DepthStencilView> CreateDepthStencilView(IDXGISwapChain * swapChain);
		};

		std::shared_ptr<IDXGISwapChain> CreateSwapChain(
			DXGI_MODE_DESC * displayMode, HWND hWnd, ID3D11Device * device, bool useMultiSample
		);
		std::shared_ptr<ID3D11Texture2D> AccessBackBuffer(IDXGISwapChain * swapChain);
		std::shared_ptr<ID3D11Device> AccessD3Device(IDXGISwapChain * swapChain);
		std::shared_ptr<ID3D11DeviceContext> AccessD3Context(ID3D11Device * device);
		std::shared_ptr<ID3D11DeviceContext> AccessD3Context(IDXGISwapChain * swapChain);
	};
	void mapping(ID3D11Resource * buffer, ID3D11DeviceContext * context, std::function<void(D3D11_MAPPED_SUBRESOURCE)> function);
};