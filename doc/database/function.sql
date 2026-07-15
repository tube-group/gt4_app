CREATE OR REPLACE FUNCTION public.api_bundle_data_t_update_func()
RETURNS TRIGGER AS $$
BEGIN
    -- 1. 删除旧数据在删除表中的历史记录
    -- 在 PostgreSQL 中，OLD 变量代表被修改前的单行数据
    DELETE FROM api_bundle_data_del_t 
    WHERE order_no = OLD.order_no 
      AND item_no = OLD.item_no 
      AND bundle_no = OLD.bundle_no;

    -- 2. 将旧数据备份到删除表中
    -- OLD.* 可以直接代表整行数据
    INSERT INTO api_bundle_data_del_t 
    SELECT OLD.*;

    -- 行级触发器在 AFTER 状态下返回 NEW 或 NULL 都可以
    RETURN NEW; 
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER api_bundle_data_t_update
    AFTER UPDATE ON api_bundle_data_t
    FOR EACH ROW
    EXECUTE FUNCTION public.api_bundle_data_t_update_func();

CREATE OR REPLACE FUNCTION public.api_bundle_data_t_delete_func()
RETURNS TRIGGER AS $$
BEGIN
    -- 1. 删除子表（管材数据表）中的关联数据
    DELETE FROM api_tube_data_t 
    WHERE order_no = OLD.order_no 
      AND item_no = OLD.item_no 
      AND bundle_no = OLD.bundle_no;

    -- 2. 删除备份表中的历史数据
    DELETE FROM api_bundle_data_del_t 
    WHERE order_no = OLD.order_no 
      AND item_no = OLD.item_no 
      AND bundle_no = OLD.bundle_no;

    -- 3. 将刚刚被删除的这一行数据（OLD）备份到删除表中
    INSERT INTO api_bundle_data_del_t 
    SELECT OLD.*;

    -- 在 AFTER DELETE 触发器中，返回 OLD 或 NULL 都可以
    RETURN OLD; 
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER api_bundle_data_t_delete
    AFTER DELETE ON api_bundle_data_t
    FOR EACH ROW
    EXECUTE FUNCTION public.api_bundle_data_t_delete_func();