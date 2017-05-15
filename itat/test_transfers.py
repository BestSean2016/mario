# -*- coding: UTF-8 -*-
# author: star
# created_at: 17-5-6 0:04


class Message(object):

    @classmethod
    def accept(cls, pl_ex_id, pl_id, node_id, status, check_status, code, str_out, str_err):
        """
        188, 0, 5, 5, 5, 0

        接受执行状态消息
        :param pl_ex_id: 流程执行id
        :param pl_id: 流程id
        :param node_id: 节点id
        :param status: 状态 if(node_id) node_status else pl_status
        :param check_status: 检测状态
        :param code: 0 正常 other 错误
        :param str_out: stdout 输出信息
        :param str_err: stderr 错误信息
        :return:
        """
        # print(pl_ex_id, pl_id, node_id, status, check_status, code, str_out, str_err)
