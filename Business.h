//
// Created by wejman on 14.05.2022.
//

#ifndef PROJ_BUSINESS_H
#define PROJ_BUSINESS_H

namespace GreedyRabbit {
    class Business {
    protected:
        int earningProgress;
    public:

        virtual int getProgressIncrement() = 0;

        virtual ~Business() = default;
    };
}


#endif //PROJ_BUSINESS_H
