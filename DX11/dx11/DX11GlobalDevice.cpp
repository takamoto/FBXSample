#include "DX11GlobalDevice.h"

#include "DX11ThinWrapper.h"

static std::weak_ptr<ID3D11Device> g_device;

namespace dx11 {
	DX11DeviceSharedGuard::DX11DeviceSharedGuard() {
		if (auto device = g_device.lock()) {
			_device = device;
		} else {
			g_device = _device = d3::init::CreateD3Device();
		}
	}

	::ID3D11Device * AccessDX11Device() {
		return g_device.lock().get();
	}
}