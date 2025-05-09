/*
 * _KData.cpp
 *
 *  Created on: 2012-9-28
 *      Author: fasiondog
 */

#include <hikyuu/serialization/KData_serialization.h>
#include <hikyuu/indicator/crt/KDATA.h>
#include "pybind_utils.h"

using namespace hku;
namespace py = pybind11;

const KRecord& (KData::*KData_getKRecord1)(size_t pos) const = &KData::getKRecord;
const KRecord& (KData::*KData_getKRecord2)(Datetime datetime) const = &KData::getKRecord;

void export_KData(py::module& m) {
    py::class_<KData>(
      m, "KData",
      "通过 Stock.getKData 获取的K线数据，由 KRecord 组成的数组，可象 list 一样进行遍历")
      .def(py::init<>())
      .def("__str__", &KData::toString)
      .def("__repr__", &KData::toString)

      .def_property_readonly("start_pos", &KData::startPos,
                             "获取在原始K线记录中对应的起始位置，如果KData为空返回0")
      .def_property_readonly(
        "end_pos", &KData::endPos,
        "获取在原始K线记录中对应范围的下一条记录的位置，如果为空返回0,其他等于lastPos + 1")
      .def_property_readonly(
        "last_pos", &KData::lastPos,
        "获取在原始K线记录中对应的最后一条记录的位置，如果为空返回0,其他等于endPos - 1")

      .def_property_readonly("open", &KData::open,
                             "返回包含开盘价的 Indicator 实例，相当于 OPEN(k)")
      .def_property_readonly("close", &KData::close,
                             "返回包含收盘价的 Indicator 实例，相当于 CLOSE(k)")
      .def_property_readonly("high", &KData::high,
                             "返回包含最高价的 Indicator 实例，相当于 HIGH(k)")
      .def_property_readonly("low", &KData::low, "返回包含最低价的 Indicator 实例，相当于 LOW(k)")
      .def_property_readonly("amo", &KData::amo, "返回包含成交金额的 Indicator 实例，相当于 AMO(k)")
      .def_property_readonly("vol", &KData::vol, "返回包含成交量的 Indicator 实例，相当于 VOL(k)")

      .def("get_datetime_list", &KData::getDatetimeList, R"(get_datetime_list(self)

        返回交易日期列表

        :rtype: DatetimeList)")

      .def("get", KData_getKRecord1, py::return_value_policy::copy, R"(get(self, pos)

        获取指定索引位置的K线记录

        :param int pos: 位置索引
        :rtype: KRecord)")

      .def("get_by_datetime", KData_getKRecord2, py::return_value_policy::copy,
           R"(get_by_datetime(self, datetime)

        获取指定时间的K线记录。

        :param Datetime datetime: 指定的日期
        :rtype: KRecord)")

      .def(
        "get_pos",
        [](const KData& self, const Datetime& d) {
            size_t pos = self.getPos(d);
            py::object ret = py::none();
            if (pos != Null<size_t>()) {
                ret = py::int_(pos);
            }
            return ret;
        },
        R"(get_pos(self, datetime)

        获取指定时间的K线记录的索引位置, 如果不在数据范围内，则返回 None
        
        :param Datetime datetime: 指定的日期
        :rtype: int)")

      .def(
        "get_pos_in_stock",
        [](const KData& self, Datetime datetime) {
            size_t pos = self.getPosInStock(datetime);
            py::object ret = py::none();
            if (pos != Null<size_t>()) {
                ret = py::int_(pos);
            }
            return ret;
        },
        R"(get_pos_in_stock(self, datetime) 
        
        获取指定时间对应的原始K线中的索引位置

        :param Datetime datetime: 指定的时间
        :return: 对应的索引位置，如果不在数据范围内，则返回 None)")

      .def("empty", &KData::empty, R"(empty(self)

        判断是否为空

        :rtype: bool)")

      .def("get_query", &KData::getQuery, py::return_value_policy::copy, R"(get_query(self)

        获取关联的查询条件

        :rtype: KQuery)")

      .def("get_stock", &KData::getStock, py::return_value_policy::copy, R"(get_stock(self)

        获取关联的Stock

        :rtype: Stock)")

      .def("get_kdata", &KData::getKData, R"(get_kdata(self, start_date, end_date)
      
        通过当前 KData 获取一个保持数据类型、复权类型不变的新的 KData（注意，不是原 KData 的子集）

        :param Datetime start: 新的起始日期
        :param Datetime end: 新的结束日期
        :rtype: KData)")

      .def("tocsv", &KData::tocsv, R"(tocsv(self, filename)

        将数据保存至CSV文件

        :param str filename: 指定保存的文件名称)")

      .def("__len__", &KData::size)

      .def(py::self == py::self)
      .def(py::self != py::self)

        DEF_PICKLE(KData);
}
