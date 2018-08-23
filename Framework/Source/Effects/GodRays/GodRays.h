/***************************************************************************
# Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#pragma once
#include <memory>
#include "Graphics/RenderGraph/RenderPass.h"
#include "API/FBO.h"
#include "Graphics/Scene/Scene.h"
#include "Graphics/FullScreenPass.h"
#include "Effects/Utils/PassFilter/PassFilter.h"

namespace Falcor
{
    class RenderContext;

    class GodRays : public RenderPass, public inherit_shared_from_this<RenderPass, GodRays>
    {
    public:
        using UniquePtr = std::unique_ptr<GodRays>;
        using SharedPtr = std::shared_ptr<GodRays>;

        static UniquePtr create(float mediumDensity = 1.0f, float mediumDecay = 0.9f, float mediumWeight = 1.0f, float exposer = 0.6f, int32_t numSamples = 200);

        void execute(RenderContext* pRenderContext, Fbo::SharedPtr pFbo);
        void execute(RenderContext* pRenderContext, const Texture::SharedPtr& pSrcTex, const Texture::SharedPtr& pSrcDepthTex, Fbo::SharedPtr pFbo);

        virtual void execute(RenderContext* pRenderContext, const RenderData* pRenderData) override;

        /** Render UI controls for bloom settings.
        \param[in] pGui GUI instance to render UI elements with
        \param[in] uiGroup Optional name. If specified, UI elements will be rendered within a named group
        */
        virtual void renderUI(Gui* pGui, const char* uiGroup = nullptr) override;

        /** Called once before compilation. Describes I/O requirements of the pass.
        The requirements can't change after the graph is compiled. If the IO requests are dynamic, you'll need to trigger compilation of the render-graph yourself.
        */
        virtual void reflect(RenderPassReflection& reflector) const override;

        void setNumSamples(int32_t numSamples);

        static UniquePtr deserialize(const RenderPassSerializer& serializer);

        void serialize(RenderPassSerializer& renderPassSerializer) override;

        virtual void setScene(const std::shared_ptr<Scene>& pScene) override { mpScene = pScene; }

    private:
        GodRays(float mediumDensity, float mediumDecay, float mediumWeight, float exposer, int32_t numSamples);
        void updateLowResTexture(const Texture::SharedPtr& pTexture);
        void createShader();

        float mMediumDensity;
        float mMediumDecay;
        float mMediumWeight;
        float mExposer = 1.0f;
        int32_t mNumSamples;
        int32_t mLightIndex = 0;
        bool mDirty = true;
        size_t mLightVarOffset = 0;
        uint32_t mOutputIndex = 0;

        GraphicsVars::SharedPtr mpVars;
        GraphicsVars::SharedPtr mpLightPassVars;
        Fbo::SharedPtr mpLightPassFbo;
        Scene::SharedPtr mpScene;
        Fbo::SharedPtr mpTargetFbo;
        Texture::SharedPtr mpLowResTexture;
        FullScreenPass::UniquePtr mpBlitPass;
        FullScreenPass::UniquePtr mpLightPass;
        ParameterBlockReflection::BindLocation mSrcTexLoc;
        ParameterBlockReflection::BindLocation mSrcDepthLoc;
        BlendState::SharedPtr mpAdditiveBlend;
        Sampler::SharedPtr mpSampler;
    };
}
