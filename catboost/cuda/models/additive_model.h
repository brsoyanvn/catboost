#pragma once

#include <ostream>

namespace NCatboostCuda {
    template <class TInner>
    class TAdditiveModel {
    public:
        TVector<TInner> WeakModels;

        TAdditiveModel() {
        }

        TAdditiveModel(const TAdditiveModel& other) = default;

        template <class TDataSet, class TCursor>
        void Append(const TDataSet& ds,
                    TCursor& cursor) const {
            for (size_t i = 0; i < WeakModels.size(); ++i) {
                WeakModels[i].Append(ds, cursor);
            }
        }

        void Rescale(const double scale) {
            for (auto& weakModel : WeakModels) {
                weakModel.Rescale(scale);
            }
        }

        void AddWeakModel(TInner&& weak) {
            WeakModels.push_back(std::move(weak));
        }

        void AddWeakModel(const TInner& weak) {
            WeakModels.push_back(weak);
        }

        const TInner& GetWeakModel(int i) const {
            return WeakModels[i];
        }

        const TInner& operator[](int i) const {
            return WeakModels[i];
        }

        size_t Size() const {
            return WeakModels.size();
        }

        void Shrink(ui32 newSize) {
            CB_ENSURE(newSize <= WeakModels.size());
            WeakModels.resize(newSize);
        }

        double Value(const TVector<float>& point) const {
            double value = 0.0;
            for (ui32 i = 0; i < WeakModels.size(); i++)
                value += (double)WeakModels[i].Value(point);
            return value;
        }

        double Value(TVector<float>::const_iterator point) const {
            double value = 0.0;
            for (ui32 i = 0; i < WeakModels.size(); i++)
                value += (double)WeakModels[i].Value(point);
            return value;
        }

        Y_SAVELOAD_DEFINE(WeakModels);
    };
}

template <class TWeak>
inline std::ostream& operator<<(std::ostream& os, const NCatboostCuda::TAdditiveModel<TWeak>& model) {
    for (ui32 i = 0; i < model.WeakModels.size(); i++) {
        if (i > 0)
            os << "+" << std::endl;
        os << model.WeakModels[i] << std::endl;
    }
    return os;
}
