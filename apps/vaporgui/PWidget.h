#pragma once

#include <string>
#include <QWidget>

namespace VAPoR {
    class ParamsBase;
    class ParamsMgr;
    class DataMgr;
}

class PDynamicMixin;
class SettingsParams;

//! \class PWidget
//! A Qt Widget that is automatically synced with the Params database.
//! The Update method must be called as per Vapor's convensions
//! All other public methods are self-explanitory. To see a demo and example
//! of how to use these widgets, see ParamsWidgetDemo

class PWidget : public QWidget {
    Q_OBJECT
    
    VAPoR::ParamsBase *_params = nullptr;
    VAPoR::ParamsMgr  *_paramsMgr = nullptr;
    VAPoR::DataMgr    *_dataMgr = nullptr;
    const std::string _tag;
    
    bool        _showBasedOnParam = false;
    std::string _showBasedOnParamTag = "";
    int         _showBasedOnParamValue;
    
    bool        _enableBasedOnParam = false;
    std::string _enableBasedOnParamTag = "";
    int         _enableBasedOnParamValue;
    
    bool _dynamicUpdateIsOn = false;
    bool _dynamicUpdateInsideGroup = false;
    
public:
    PWidget(const std::string &tag, QWidget *widget);
    void Update(VAPoR::ParamsBase *params, VAPoR::ParamsMgr *paramsMgr = nullptr, VAPoR::DataMgr *dataMgr = nullptr);
    const std::string &GetTag() const;
    
    PWidget *ShowBasedOnParam(const std::string &tag, int whenEqualTo = true);
    PWidget *EnableBasedOnParam(const std::string &tag, int whenEqualTo = true);
    PWidget *SetTooltip(const std::string &text);
    void setToolTip(const QString &) = delete;
    
protected:
    virtual void updateGUI() const = 0;
    virtual bool requireParamsMgr() const { return false; }
    virtual bool requireDataMgr()   const { return false; }
    
    VAPoR::ParamsBase *getParams() const;
    VAPoR::ParamsMgr  *getParamsMgr() const;
    VAPoR::DataMgr    *getDataMgr() const;
    SettingsParams    *getSettingsParams() const;
    
    void setParamsDouble(double v);
    void setParamsLong(long v);
    void setParamsString(const std::string &v);
    double      getParamsDouble() const;
    long        getParamsLong()   const;
    std::string getParamsString() const;
    
private:
    void dynamicUpdateBegin();
    void dynamicUpdateFinish();
    void _setParamsDouble(double v);
    void _setParamsLong(long v);
    void _setParamsString(const std::string &v);
    
    friend class PDynamicMixin;
};
