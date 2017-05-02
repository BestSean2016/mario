# -*- coding: UTF-8 -*-
# author: star
# created_at: 17-4-16 10:21
import json
import time


# from django.core.cache import cache

# from bill.consumers import DefaultConsumer
# from pipeline.models import Node
# from execute.models import ExecLog
# from execute.serializers import ExecLogSerializers
# from pipeline.models import Pipeline
# from bill.query_mysql import query_node_by_id


class Message(object):
    pl_id = pl_ex_id = node_id = code = 0
    status = check_status = str_out = str_err = None
    log = pipeline = None

    # @classmethod
    # def set_pipeline(cls, pl_id):
        # cls.pipeline = Pipeline.objects.find(pl_id)

    map = {
        -1: 'st_unknow',  # Î´Öª×´Ì¬
        0: 'st_initial',  # Á÷³Ì³õÊ¼»¯
        1: 'st_checking',  # ÕýÔÚ¼ì²â
        2: 'st_checked_err',  # Á÷³Ì¼ì²â´íÎó
        3: 'st_checked_serr',  # ½Úµã¼ì²â£­½Å±¾´íÎó
        4: 'st_checked_herr',  # ½Úµã¼ì²â£­Ö÷»ú´íÎó
        5: 'st_checked_ok',  # ¼ì²â³É¹¦
        6: 'st_running',  # ÕýÔÚÖ´ÐÐ
        7: 'st_error',  # Ö´ÐÐ´íÎó
        8: 'st_timeout',  # Ö´ÐÐ³¬Ê±
        9: 'st_succeed',  # Ö´ÐÐ³É¹¦
        10: 'st_waiting_for_confirm',  # µÈ´ýÓÃ»§È·ÈÏ
        11: 'st_stopped',  # Í£Ö¹Ö´ÐÐ
        12: 'st_stopping',  # ÕýÔÚÍ£Ö¹
        13: 'st_paused',  # ÔÝÍ£Ö´ÐÐ
        14: 'st_pausing',  # ÕýÔÚÔÝÍ£
        15: 'st_waiting_for_input',  # µÈ´ýÓÃ»§ÊäÈë
        16: 'st_running_one',  # Ö´ÐÐµ¥¸ö½Úµã/ÖØ×ö
        17: 'st_run_one_ok',  # Ö´ÐÐµ¥¸ö½Úµã/ÖØ×ö ³É¹¦
        18: 'st_run_one_err',  # Ö´ÐÐµ¥¸ö½Úµã/ÖØ×ö Ê§°Ü
        19: 'st_confirm_refused',  # ÓÃ»§¾Ü¾ø
    }

    @classmethod
    def accept(cls, pl_ex_id, pl_id, node_id, status, check_status, code, str_out, str_err):
        """
        0, 188, 5, 5, 5, 0
        ½ÓÊÜÖ´ÐÐ×´Ì¬ÏûÏ¢
        :param pl_ex_id: Á÷³ÌÖ´ÐÐid
        :param pl_id: Á÷³Ìid
        :param node_id: ½Úµãid
        :param status: ×´Ì¬ if(node_id) node_status else pl_status
        :param check_status: ¼ì²â×´Ì¬
        :param code: 0 Õý³£ other ´íÎó
        :param str_out: stdout Êä³öÐÅÏ¢
        :param str_err: stderr ´íÎóÐÅÏ¢
        :return:
        """
        print pl_id, pl_ex_id, node_id, status, check_status, code, str_out, str_err

        # from common.message import Message as TestMessage
        # m = TestMessage(
        #     pl_id=pl_id, pl_ex_id=pl_ex_id, node_id=node_id, status=status, check_status=check_status, code=code)
        #
        # print m.write_log().send_data()
        cls.pl_id = pl_id
        cls.pl_ex_id = pl_ex_id
        cls.node_id = node_id
        # if node_id:
        #     query_node_by_id(node_id)
        #     cls.node = cache.get("node:%d" % node_id)
        cls.status = status
        cls.check_status = check_status
        cls.code = code
        cls.str_out = str_out
        cls.str_err = str_err
        cls.static_send()

    @classmethod
    def static_send(cls):
        """
        Èç¹ûlog´æÔÚÔòÍÆËÍÊý¾Ýµ½Ç°¶Ë
        1 ½Úµã×´Ì¬ {'log': cls.writeLog(), 'node': {'status': cls.status, 'id': int(cls.node_id)}}
        2 Á÷³Ì×´Ì¬ {'log': cls.writeLog(), 'status':cls.status, 'check_status':cls.check_status}
        Õý³£ÔËÐÐ£º
        ÍÆËÍµ±Ç°Á÷³Ì·¿¼ä
        ´íÎóÔËÐÐ£º
        ÍÆËÍµ±Ç°Á÷³Ì·¿¼ä
        Ôö¼Ó¸æ¾¯£­ÔÚ¸æ¾¯Êý¾Ý±ä»¯Ê±×Ô¶¯´¥·¢ÍÆËÍdefault·¿¼äµÄ»úÖÆ
        :return:
        """
        log = cls.writeLog()
        text = '{"node": {node}, "status": {status}, "check_status": {check_status}, "log": {log}}'
        if log:
            text = text.replace('{log}', log)
        else:
            text = text.replace('{log}', '{}')

        if int(cls.node_id) > 0:
            text = text.replace('{node}', '{"status": '+str(cls.status)+', "id": '+str(int(cls.node_id))+'}')
        else:
            text = text.replace('{node}', '{}')

        if int(cls.node_id) == 0:
            text = text.replace('{status}', str(cls.status))
            text = text.replace('{check_status}', str(cls.check_status))
        else:
            text = text.replace('{status}', '{}')
            text = text.replace('{check_status}', '{}')

        # DefaultConsumer.group_send(u'chat-%d' % cls.pl_id, text)
        # DefaultConsumer.group_send(u'default', text)
        # DefaultConsumer.group_send(u'default', cls.get_result_info())
        # if int(cls.node_id) > 0 and (cls.status == 7 or cls.status == 8):
        #     luigi = cache.get("user_pipeline_%d" % cls.pl_id)
        #     luigi.stop()

    @classmethod
    def writeLog(cls):
        text = cls.get_result_info()
        if text:
            # log = ExecLog.objects.create(
            #     ex_pl_id=cls.pl_id,
            #     node_id=int(cls.node_id),
            #     result_info=cls.get_result_info(),
            #     status=cls.code)
            # return log
            #local_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
            #time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
            # local_time = time.time()
            return u'{"ex_pl_id": 1, "node_id": '+str(int(cls.node_id))+', "result_info": "'+text+'", "status": '+str(cls.code)+', "created_at": '+str(time.time())+'}'
        return 0

    @classmethod
    def get_result_info(cls):
        text = ''
        if int(cls.node_id) == 0:
            if cls.status == 0 and cls.check_status == 1:
                text = u'Á÷³Ì%d ÕýÔÚ¿ªÊ¼¼ì²â' % int(cls.pl_ex_id)
            elif cls.status == 0 and cls.check_status == 2:
                text = u'Á÷³Ì%d ¼ì²â´íÎó Í£Ö¹Ö´ÐÐ' % int(cls.pl_ex_id)
            elif cls.status == 0 and cls.check_status == 5:
                text = u'Á÷³Ì%d ¼ì²â³É¹¦£¬¼´½«¿ªÊ¼Ö´ÐÐ' % int(cls.pl_ex_id)
            elif cls.status == 9 and cls.check_status == 5:
                text = u'Á÷³Ì%d Ö´ÐÐÍê³É' % int(cls.pl_ex_id)
            elif cls.status == 11 and cls.check_status == 5:
                text = u'Á÷³Ì%d ÒÑÍ£Ö¹' % int(cls.pl_ex_id)
            elif cls.status == 12 and cls.check_status == 5:
                text = u'Á÷³Ì%d ÕýÔÚÍ£Ö¹....' % int(cls.pl_ex_id)
            elif cls.status == 13 and cls.check_status == 5:
                text = u'Á÷³Ì%d Ö´ÐÐ³¬Ê±, ÒÑÔÝÍ£' % int(cls.pl_ex_id)
            elif cls.status == 14 and cls.check_status == 5:
                text = u'Á÷³Ì%d ÕýÔÚÔÝÍ£....' % int(cls.pl_ex_id)
            # elif cls.status == 6 and cls.check_status == 5:
            #     text = u'Á÷³Ì%d ÕýÔÚÖ´ÐÐ....' % int(cls.pl_ex_id)

        else:
            if cls.status == 1:
                text = u'Á÷³Ì%d %d ¿ªÊ¼¼ì²â' % (int(cls.pl_ex_id), int(cls.node_id))
            elif cls.status == 3:
                text = u'Á÷³Ì%d %d ½Å±¾´íÎó' % (int(cls.pl_ex_id), int(cls.node_id))
            elif cls.status == 4:
                text = u'Á÷³Ì%d %d Ö÷»ú·ÃÎÊ²»Í¨' % (int(cls.pl_ex_id), int(cls.node_id))
            elif cls.status == 5:
                text = u'Á÷³Ì%d %d ¼ì²â³É¹¦' % (int(cls.pl_ex_id), int(cls.node_id))
            elif cls.status == 6:
                text = u'Á÷³Ì%d %d ¿ªÊ¼Ö´ÐÐ' % (int(cls.pl_ex_id), int(cls.node_id))
            elif cls.status == 9:
                text = u'Á÷³Ì%d %d Ö´ÐÐ³É¹¦' % (int(cls.pl_ex_id), int(cls.node_id))
            elif cls.status == 10:
                text = u'Á÷³Ì%d %d µÈ´ýÓÃ»§È·ÈÏ' % (int(cls.pl_ex_id), int(cls.node_id))
            elif cls.status == 7:
                text = u'Á÷³Ì%d %d Ö´ÐÐ´íÎó' % (int(cls.pl_ex_id), int(cls.node_id))
            elif cls.status == 8:
                text = u'Á÷³Ì%d %d Ö´ÐÐ³¬Ê±' % (int(cls.pl_ex_id), int(cls.node_id))
        return text
