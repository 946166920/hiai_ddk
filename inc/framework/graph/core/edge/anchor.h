/**
 * Copyright 2019-2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GE_ANCHOR_H
#define GE_ANCHOR_H

#include <memory>
#include <vector>
#include <string>

// api/framework
#include "graph/types.h"
#include "graph/graph_api_export.h"
#include "graph/debug/ge_error_codes.h"

// inc/framework
#include "framework/graph/core/node/node_fwd.h"
#include "framework/graph/core/edge/anchor_fwd.h"
#include "framework/graph/utils/range_vistor.h"
#include "framework/graph/debug/ge_util.h"

namespace ge {
class GRAPH_API_EXPORT Anchor : public std::enable_shared_from_this<Anchor> {
    friend class AnchorUtils;

public:
    using TYPE = std::string;
    template<class T>
    using Vistor = RangeVistor<T, std::shared_ptr<ConstAnchor>>;

public:
    Anchor(NodePtr ownerNode, int idx);

    virtual ~Anchor() = default;

    // whether the two anchor is equal
    virtual bool Equal(const AnchorPtr& anchor) const = 0;

protected:
    virtual bool IsTypeOf(TYPE type) const;

public:
    // get all peer anchors connected to current anchor
    Vistor<AnchorPtr> GetPeerAnchors() const;

    // get the anchor belong to which node
    NodePtr GetOwnerNode() const;

    // remove all links with the anchor
    void UnlinkAll();

    // remove link with the given anchor
    GraphErrCodeStatus Unlink(AnchorPtr peer);

    // replace peer with new peers
    GraphErrCodeStatus ReplacePeer(AnchorPtr oldPeer, AnchorPtr firstPeer, AnchorPtr secondPeer);

    // judge if the anchor is linked with the given anchor
    bool IsLinkedWith(AnchorPtr peer);

    int GetIdx() const;

protected:
    // all peer anchors connected to current anchor
    std::vector<std::weak_ptr<Anchor>> peerAnchors_;
    // the owner node of anchor
    std::weak_ptr<Node> ownerNode_;
    // the index of input related to current anchor
    int idx_;

protected:
    template<class T>
    static Anchor::TYPE TypeOf()
    {
        static_assert(std::is_base_of<Anchor, T>::value, "T must be a Anchor!");
        return __PRETTY_FUNCTION__;
    }

public:
    template<class T>
    static std::shared_ptr<T> DynamicAnchorCast(AnchorPtr anchorPtr)
    {
        static_assert(std::is_base_of<Anchor, T>::value, "T must be a Anchor!");
        if (!anchorPtr || !anchorPtr->IsTypeOf<T>()) {
            return nullptr;
        }
        return std::static_pointer_cast<T>(anchorPtr);
    }

public:
    template<typename T>
    bool IsTypeOf()
    {
        return IsTypeOf(TypeOf<T>());
    }
};

class GRAPH_API_EXPORT DataAnchor : public Anchor {
    friend class AnchorUtils;

public:
    DataAnchor(NodePtr ownerNode, int idx);

    ~DataAnchor() override
    {
    }

    bool IsTypeOf(TYPE type) const override;

private:
    Format format_ {FORMAT_NCHW};
    AnchorStatus status_ {ANCHOR_SUSPEND};
};

class GRAPH_API_EXPORT InDataAnchor : public DataAnchor {
    friend class OutDataAnchor;

public:
    InDataAnchor(NodePtr ownerNode, int idx);

    ~InDataAnchor() override
    {
    }

    // get  source out data anchor
    OutDataAnchorPtr GetPeerOutAnchor() const;

    // build connection from OutDataAnchor to InDataAnchor
    GraphErrCodeStatus LinkFrom(OutDataAnchorPtr src);

    bool Equal(const AnchorPtr& anchor) const override;

protected:
    bool IsTypeOf(TYPE type) const override;
};

class GRAPH_API_EXPORT OutDataAnchor : public DataAnchor {
    friend class InDataAnchor;

    friend class AnchorUtils;

public:
    template<class T>
    using Vistor = RangeVistor<T, std::shared_ptr<ConstAnchor>>;

public:
    OutDataAnchor(NodePtr ownerNode, int idx);

    ~OutDataAnchor() override
    {
    }

    // get dst in data anchor(one or more)
    Vistor<InDataAnchorPtr> GetPeerInDataAnchors() const;

    // build connection from OutDataAnchor to InDataAnchor
    GraphErrCodeStatus LinkTo(InDataAnchorPtr dest);

    bool Equal(const AnchorPtr& anchor) const override;

protected:
    bool IsTypeOf(TYPE type) const override;
};

class GRAPH_API_EXPORT ControlAnchor : public Anchor {
public:
    explicit ControlAnchor(NodePtr ownerNode);

    ~ControlAnchor() override
    {
    }

protected:
    bool IsTypeOf(TYPE type) const override;
};

class GRAPH_API_EXPORT InControlAnchor : public ControlAnchor {
    friend class OutControlAnchor;

    friend class OutDataAnchor;

public:
    explicit InControlAnchor(NodePtr ownerNode);

    ~InControlAnchor() override
    {
    }

    // get  source out control anchors
    Vistor<OutControlAnchorPtr> GetPeerOutControlAnchors() const;

    // build connection from OutControlAnchor to InControlAnchor
    GraphErrCodeStatus LinkFrom(OutControlAnchorPtr src);

    bool Equal(const AnchorPtr& anchor) const override;

protected:
    bool IsTypeOf(TYPE type) const override;
};

class GRAPH_API_EXPORT OutControlAnchor : public ControlAnchor {
    friend class InControlAnchor;

public:
    template<class T>
    using Vistor = RangeVistor<T, std::shared_ptr<ConstAnchor>>;

public:
    explicit OutControlAnchor(NodePtr ownerNode);

    ~OutControlAnchor() override
    {
    }

    // get dst in control anchor(one or more)
    Vistor<InControlAnchorPtr> GetPeerInControlAnchors() const;

    // build connection from OutControlAnchor to InControlAnchor
    GraphErrCodeStatus LinkTo(InControlAnchorPtr dest);

    bool Equal(const AnchorPtr& anchor) const override;

protected:
    bool IsTypeOf(TYPE type) const override;
};
} // namespace ge
#endif // GE_ANCHOR_H
