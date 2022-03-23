#pragma once
#ifndef UPLOADBUFFER_H
#define UPLOADBUFFER_H

#include "D3DMethods.h"

template<typename T>
class UploadBuffer
{
public:
    UploadBuffer(Microsoft::WRL::ComPtr<ID3D12Device> device, UINT elementCount, bool isConstantBuffer) :
        mIsConstantBuffer(isConstantBuffer)
    {
        //如果不是常量缓冲区，则直接计算缓存大小
        mElementByteSize = sizeof(T);

        // typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
        // UINT64 OffsetInBytes; // multiple of 256
        // UINT   SizeInBytes;   // multiple of 256
        // } D3D12_CONSTANT_BUFFER_VIEW_DESC;

        if(isConstantBuffer)//如果是常量缓冲区，则以256的倍数计算缓存大小
            mElementByteSize = MathHelpTool::alignedBytesAs256(sizeof(T));

        //创建上传堆和资源
        ThrowIfFailed(device->CreateCommittedResource(
            GetTypePointer(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)),
            D3D12_HEAP_FLAG_NONE,
            GetTypePointer(CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&mUploadBuffer)));

        //返回欲更新资源的指针
        ThrowIfFailed(mUploadBuffer->Map(0,             //子资源索引，对于缓冲区来说，他的子资源就是自己
            nullptr,                                    //对整个资源进行映射
            reinterpret_cast<void**>(&mMappedData)));   //返回待映射资源数据的目标内存块

        // We do not need to unmap until we are done with the resource.  However, we must not write to
        // the resource while it is in use by the GPU (so we must use synchronization techniques).
    }

    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
    ~UploadBuffer()
    {
        if(mUploadBuffer != nullptr)
            mUploadBuffer->Unmap(0, nullptr);//取消映射

        mMappedData = nullptr;               //释放映射内存
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> Resource() const
    {
        return mUploadBuffer;
    }

    void CopyData(int elementIndex, const T& data)
    {
        memcpy(&mMappedData[elementIndex*mElementByteSize], &data, sizeof(T));
    }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
    BYTE* mMappedData = nullptr;

    UINT mElementByteSize = 0;
    bool mIsConstantBuffer = false;
};

#endif // !UPLOADBUFFER