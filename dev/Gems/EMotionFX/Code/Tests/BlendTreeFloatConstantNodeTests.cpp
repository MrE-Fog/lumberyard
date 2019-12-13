/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include <AzCore/Math/Random.h>
#include <EMotionFX/Source/AnimGraph.h>
#include <EMotionFX/Source/AnimGraphBindPoseNode.h>
#include <EMotionFX/Source/AnimGraphStateMachine.h>
#include <EMotionFX/Source/BlendTree.h>
#include <EMotionFX/Source/BlendTreeBlend2Node.h>
#include <EMotionFX/Source/BlendTreeFloatConstantNode.h>
#include <EMotionFX/Source/EMotionFXManager.h>
#include <limits.h>
#include <Tests/AnimGraphFixture.h>

namespace EMotionFX
{
    class BlendTreeFloatConstantNodeFixture
        : public AnimGraphFixture
    {
    public:
        void ConstructGraph() override
        {
            AnimGraphFixture::ConstructGraph();

            // Create the blend tree.
            m_blendTree = aznew BlendTree();
            m_animGraph->GetRootStateMachine()->AddChildNode(m_blendTree);
            m_animGraph->GetRootStateMachine()->SetEntryState(m_blendTree);

            // Add nodes to blend tree.
            BlendTreeFinalNode* finalNode = aznew BlendTreeFinalNode();
            m_blendTree->AddChildNode(finalNode);
            m_blend2Node = aznew BlendTreeBlend2Node();
            m_blendTree->AddChildNode(m_blend2Node);
            m_floatConstantNode = aznew BlendTreeFloatConstantNode();
            m_blendTree->AddChildNode(m_floatConstantNode);
            AnimGraphBindPoseNode* bindPoseNode = aznew AnimGraphBindPoseNode();
            m_blendTree->AddChildNode(bindPoseNode);

            // Connect the nodes.
            m_blend2Node->AddConnection(bindPoseNode, AnimGraphBindPoseNode::PORTID_OUTPUT_POSE, BlendTreeBlend2Node::INPUTPORT_POSE_A);
            m_blend2Node->AddConnection(bindPoseNode, AnimGraphBindPoseNode::PORTID_OUTPUT_POSE, BlendTreeBlend2Node::INPUTPORT_POSE_B);
            m_blend2Node->AddConnection(m_floatConstantNode, BlendTreeFloatConstantNode::PORTID_OUTPUT_RESULT, BlendTreeBlend2Node::INPUTPORT_WEIGHT);
            finalNode->AddConnection(m_blend2Node, BlendTreeBlend2Node::PORTID_OUTPUT_POSE, BlendTreeFinalNode::PORTID_INPUT_POSE);
        }

    protected:
        BlendTreeFloatConstantNode* m_floatConstantNode = nullptr;
        BlendTreeBlend2Node* m_blend2Node = nullptr;
        BlendTree* m_blendTree = nullptr;
    };

    TEST_F(BlendTreeFloatConstantNodeFixture, NodeOutputsCorrectFloat)
    {
        // Test max float constant node value.
        m_floatConstantNode->SetValue(FLT_MAX);
        GetEMotionFX().Update(1.0f / 60.0f);
        EXPECT_FLOAT_EQ(m_floatConstantNode->
            GetOutputFloat(m_animGraphInstance, BlendTreeFloatConstantNode::PORTID_OUTPUT_RESULT)->GetValue(), FLT_MAX);

        // Test min float constant node value.
        m_floatConstantNode->SetValue(FLT_MIN);
        GetEMotionFX().Update(1.0f / 60.0f);
        EXPECT_FLOAT_EQ(m_floatConstantNode->
            GetOutputFloat(m_animGraphInstance, BlendTreeFloatConstantNode::PORTID_OUTPUT_RESULT)->GetValue(), FLT_MIN);

        // Test 10 random float constant node value in range of [-5,5).
        AZ::SimpleLcgRandom random;
        random.SetSeed(875960);
        for (AZ::u32 i = 0; i < 10; ++i)
        {
            const float randomFloat = random.GetRandomFloat() * 10.0f - 5.0f;
            m_floatConstantNode->SetValue(randomFloat);
            GetEMotionFX().Update(1.0f / 60.0f);
            EXPECT_FLOAT_EQ(m_floatConstantNode->GetOutputFloat(m_animGraphInstance,
                BlendTreeFloatConstantNode::PORTID_OUTPUT_RESULT)->GetValue(), randomFloat);
        }
    }
} // end namespace EMotionFX