class Path2D {
    SkPathBuilder builder;
    public:
    static constexpr uint64_t MAGIC = 0x0430F69F;
    uint64_t magic = MAGIC;

    Path2D() = default;
    Path2D(Path2D& target) {
        builder = target();
    };
    Path2D(Path2D* target) {
        if (target) builder = (*target)();
    };

    void moveTo(int x, int y) {
        builder.moveTo(x,y);
    }
    void lineTo(int x, int y) {
        builder.lineTo(x,y);
    }
    
    void arc(float x1, float y1, float radius, float startAngle, float sweepangle) {
        builder.addArc(SkRect::MakeXYWH(x1 - radius,y1 - radius,radius * 2,radius * 2), startAngle*57.29577958f, sweepangle*57.29577958f);
    }

    void ellipse(
        float x,
        float y,
        float radiusX,
        float radiusY,
        float rotation,
        float startAngle,
        float endAngle,
        bool counterclockwise
    )   {
        if (radiusX < 0 || radiusY < 0) {
            return;
        }
        // Normalize the angle direction to match Canvas behavior.
        float delta = endAngle - startAngle;

        constexpr float twoPi = 2.0f * SK_FloatPI;

        if (!counterclockwise) {
            while (delta < 0)
                delta += twoPi;

            if (delta > twoPi)
                delta = twoPi;
        } else {
            while (delta > 0)
                delta -= twoPi;

            if (delta < -twoPi)
                delta = -twoPi;
        }

        // Nothing to draw.
        if (delta == 0.0f)
            return;

        // Canvas treats an arc spanning >= 2π as a complete ellipse.
        if (!counterclockwise && delta >= twoPi)
            delta = twoPi;

        if (counterclockwise && delta <= -twoPi)
            delta = -twoPi;

        // Build the ellipse in its own coordinate system.
        //
        // The ellipse is centered at (x, y), with the rotation applied
        // around its center.
        SkMatrix rotationMatrix =
            SkMatrix::RotateDeg(rotation * 180.0f / SK_FloatPI, {x, y});

        // Number of segments isn't actually required if using arcTo,
        // because SkPathBuilder handles the conic approximation.
        //
        // We construct the ellipse using cubic Bézier segments.
        constexpr int maxSegments = 64;

        int segments = static_cast<int>(
            std::ceil(std::abs(delta) / (SK_FloatPI / 2.0f))
        );

        segments = std::min(segments, maxSegments);

        float segmentAngle = delta / segments;

        auto transformPoint = [&](float px, float py) -> SkPoint {
            return rotationMatrix.mapPoint({px, py});
        };

        float angle = startAngle;

        for (int i = 0; i < segments; ++i) {
            float nextAngle = angle + segmentAngle;

            float alpha = 4.0f / 3.0f *
                std::tan((nextAngle - angle) / 4.0f);

            float cosA = std::cos(angle);
            float sinA = std::sin(angle);

            float cosB = std::cos(nextAngle);
            float sinB = std::sin(nextAngle);

            // Point on ellipse.
            SkPoint p0 = {
                x + radiusX * cosA,
                y + radiusY * sinA
            };

            // Tangent at p0.
            SkPoint t0 = {
                -radiusX * sinA,
                radiusY * cosA
            };

            // Point on ellipse.
            SkPoint p3 = {
                x + radiusX * cosB,
                y + radiusY * sinB
            };

            // Tangent at p3.
            SkPoint t1 = {
                -radiusX * sinB,
                radiusY * cosB
            };

            SkPoint p1 = {
                p0.x() + alpha * t0.x(),
                p0.y() + alpha * t0.y()
            };

            SkPoint p2 = {
                p3.x() - alpha * t1.x(),
                p3.y() - alpha * t1.y()
            };

            p0 = transformPoint(p0.x(), p0.y());
            p1 = transformPoint(p1.x(), p1.y());
            p2 = transformPoint(p2.x(), p2.y());
            p3 = transformPoint(p3.x(), p3.y());

            if (i == 0) {
                builder.moveTo(p0);
            }

            builder.cubicTo(p1, p2, p3);

            angle = nextAngle;
        }
    }
    
    void arcTo(float x1, float y1, float x2, float y2, float radius) {
        builder.arcTo({x1,y1},{x2,y2},radius);
    }
    
    void bezierTo(float x1, float y1, float x2, float y2, float x3, float y3) {
        builder.cubicTo(x1,y1,x2,y2,x3,y3);
    }
    void quadraticCurveTo(float x1, float y1, float x2, float y2) {
        builder.quadTo(x1,y1,x2,y2);
    }

    void rect(float x, float y, float w, float h) {
        builder.addRect(SkRect::MakeXYWH(x,y,w,h));
    }
    void roundRect(float x, float y, float w, float h, float radii) {
        auto rect = SkRect::MakeXYWH(x,y,w,h);

        SkVector radiiV[4] = {
            {h,h}, // top-left
            {h,h}, // top-right
            {h,h}, // bottom-right
            {h,h}  // bottom-left
        };
        SkRRect rRect;
        rRect.setRectRadii(rect,radiiV);
        builder.addRRect(rRect);
    }
    void roundRect_array(float x, float y, float w, float h, float* radii, int len) {
        SkVector* radiiV;
        switch(len) {
            case 1: {
                radiiV = new SkVector[4]{
                    {radii[0],radii[0]}, // top-left
                    {radii[0],radii[0]}, // top-right
                    {radii[0],radii[0]}, // bottom-right
                    {radii[0],radii[0]}  // bottom-left
                };
                break;
            }
            case 2: {
                radiiV = new SkVector[4]{
                    {radii[0],radii[0]}, // top-left
                    {radii[1],radii[1]}, // top-right
                    {radii[0],radii[0]}, // bottom-right
                    {radii[1],radii[1]}  // bottom-left
                };
                break;
            }
            case 3: {
                radiiV = new SkVector[4]{
                    {radii[0],radii[0]}, // top-left
                    {radii[1],radii[1]}, // top-right
                    {radii[2],radii[2]}, // bottom-right
                    {radii[1],radii[1]}  // bottom-left
                };
                break;
            }
            case 4: {
                radiiV = new SkVector[4]{
                    {radii[0],radii[0]}, // top-left
                    {radii[1],radii[1]}, // top-right
                    {radii[2],radii[2]}, // bottom-right
                    {radii[3],radii[3]}  // bottom-left
                };
                break;
            }
            default: return;
        }
        if (!radiiV) return;
        auto rect = SkRect::MakeXYWH(x,y,w,h);
        SkRRect rRect;
        rRect.setRectRadii(rect, radiiV);
        builder.addRRect(rRect);
    }

    void close() {
        builder.close();
    }

    void reset(){
        builder.reset();
    }

    SkPath operator()(){
        return builder.snapshot();
    }
};

extern "C" {
    WINDOWS_EXPORT void* path2d_create() {
        return new Path2D();
    }

    WINDOWS_EXPORT void path2d_destroy(void* path2DObj) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        delete obj;
    }

    WINDOWS_EXPORT void* path2d_create_from_copy(void* path2DObj) {
        Path2D* obj = validated<Path2D>(path2DObj);
        return new Path2D(obj);
    }

    WINDOWS_EXPORT void path2d_begin(void* path2DObj) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        if (obj == nullptr) return;
        obj->reset();
    }
    
    WINDOWS_EXPORT void path2d_move_to(void* path2DObj, int x, int y) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;
        obj->moveTo(x,y);
    }

    WINDOWS_EXPORT void path2d_line_to(void* path2DObj, int x, int y) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;
        obj->lineTo(x,y);
    }
    
    WINDOWS_EXPORT void path2d_arc(void* path2DObj, float x1, float y1, float radius, float startAngle, float sweepangle) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;
        obj->arc(x1,y1,radius,startAngle,sweepangle);
    }

    WINDOWS_EXPORT void path2d_ellipse(
        void* path2DObj,
        float x,
        float y,
        float radiusX,
        float radiusY,
        float rotation,
        float startAngle,
        float endAngle,
        bool counterclockwise
    )   {
        if (radiusX < 0 || radiusY < 0) {
            return;
        }
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;

        obj->ellipse(x,y,radiusX,radiusY,rotation,startAngle,endAngle,counterclockwise);
    }
    
    WINDOWS_EXPORT void path2d_arc_to(void* path2DObj, float x1, float y1, float x2, float y2, float radius) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;
        obj->arcTo(x1,y1,x2,y2,radius);
    }
    
    WINDOWS_EXPORT void path2d_bezier_to(void* path2DObj, float x1, float y1, float x2, float y2, float x3, float y3) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;
        obj->bezierTo(x1,y1,x2,y2,x3,y3);
    }
    WINDOWS_EXPORT void path2d_quad_to(void* path2DObj, float x1, float y1, float x2, float y2) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;
        obj->quadraticCurveTo(x1,y1,x2,y2);
    }
    
    WINDOWS_EXPORT void path2d_close(void* path2DObj) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;
        obj->close();
    }
    WINDOWS_EXPORT void path2d_rect(void* path2DObj, float x, float y, float w, float h, float radii) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;
        obj->roundRect(x,y,w,h,radii);
    }
    WINDOWS_EXPORT void path2d_roundrect(void* path2DObj, float x, float y, float w, float h, float radii) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;
        obj->roundRect(x,y,w,h,radii);
    }
    WINDOWS_EXPORT void path2d_roundrect_array(void* path2DObj, float x, float y, float w, float h, float* radii, int len) {
        if (!path2DObj) return;
        Path2D* obj = validated<Path2D>(path2DObj);
        
        if (obj == nullptr) return;
        obj->roundRect_array(x,y,w,h,radii,len);
    }
}