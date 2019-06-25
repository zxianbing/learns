#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/param.h>
#include <linux/stat.h>
#include <linux/reboot.h>
#include <linux/string.h>
#include "sprd_battery.h"
#include "fan54015.h"

#undef FAN54015_DEBUG
#define FAN54015_DEBUG
#ifdef FAN54015_DEBUG
#define SPRDCHG_DEBUG(format, arg...) pr_info("sprd 54015: " format, ## arg)
#else
#define SPRDCHG_DEBUG(format, arg...)
#endif

static struct sprdbat_drivier_data *bat_drv_data;

void sprdchg_fan54015_set_vindpm(int vin)
{
	unsigned char reg_val = 0x0;

	SPRDCHG_DEBUG("%s vin =%d\n", __func__, vin);

	if (vin <= 4213)
		reg_val = 0x0;
	else if (vin >= 4773)
		reg_val = 0x7;
	else
		reg_val = (vin - 4213) / 80 + 1;

	/* Limit VBUS to 4.61v,avoid appear vbus por */
	reg_val = 0x5;
	fan54015_set_vindpm(reg_val);
}

void sprdchg_fan54015_termina_cur_set(uint32_t cur)
{
	unsigned char reg_val = 0x0;
	unsigned char vendor_id2 = 0;
	
	SPRDCHG_DEBUG("%s cur =%d\n", __func__, cur);
	vendor_id2 = fan54015_get_vendor_id2();
	if(vendor_id2 == VENDOR_ID2_ETA6937){
		if (cur <= 50){
			reg_val = 0x0;
		}
		else if (cur >= 400){
			reg_val = 0x7;
		}
		else{
			reg_val = (cur - 50) / 50;	
		}	
	}		
	else{
		
		if (cur <= 49){
			reg_val = 0x0;
		}
		else if (cur >= 388){
			reg_val = 0x7;
		}
		else{
			reg_val = (cur - 48) / 48;
		}
	}
	fan54015_termina_cur_set(reg_val);
}

void sprdchg_fan54015_termina_vol_set(uint32_t vol)
{
	unsigned char reg_val = 0x0;
	unsigned char vendor_id2 = 0;
	SPRDCHG_DEBUG("%s vol =%d\n", __func__, vol);
	
	vendor_id2 = fan54015_get_vendor_id2();
	if(vendor_id2 == VENDOR_ID2_ETA6937){
		if (vol <= 3500){
			reg_val = 0x0;
		}
		else if (vol >= 4440){
			reg_val = 0x2f;
		}
		else{
			reg_val = (vol - 3500) / 20;
		}
	}
	else{
		if (vol <= 3500)
			reg_val = 0x0;
		else if (vol >= 4440)
			reg_val = 0x2e;
		else
			reg_val = (vol - 3499) / 20;
	}		
	fan54015_termina_vol_set(reg_val);
}

void sprdchg_fan54015_set_safety_vol(int vol)
{
	unsigned char reg_val = 0x0;

	SPRDCHG_DEBUG("%s vol =%d\n", __func__, vol);

	if (vol < 4200)
		vol = 4200;
	if (vol > 4440)
		vol = 4440;
	reg_val = (vol - 4200) / 20 + 1;

	fan54015_set_safety_vol(reg_val);
}

void sprdchg_fan54015_set_safety_cur(int cur)
{
	unsigned char reg_val = ISAFE;

	SPRDCHG_DEBUG("%s cur = max\n", __func__);
	fan54015_set_safety_cur(reg_val);
}

unsigned char sprdchg_fan54015_cur2reg(uint32_t cur)
{
	unsigned char reg_val;

	if (cur < 650)
		reg_val = 0;
	if ((cur >= 650) && (cur < 750))
		reg_val = 1;
	if ((cur >= 750) && (cur < 850))
		reg_val = 2;
	if ((cur >= 850) && (cur < 1050))
		reg_val = 3;
	if ((cur >= 1050) && (cur < 1150))
		reg_val = 4;
	if ((cur >= 1150) && (cur < 1350))
		reg_val = 5;
	if ((cur >= 1350) && (cur < 1450))
		reg_val = 6;
	if (cur >= 1450)
		reg_val = 7;
	return reg_val;
}

unsigned int sprdchg_fan54015_reg2cur(unsigned char reg_val)
{
	unsigned int cur = 0;

	switch (reg_val) {
	case 0:
		cur = 550;
		break;
	case 1:
		cur = 650;
		break;
	case 2:
		cur = 750;
		break;
	case 3:
		cur = 850;
		break;
	case 4:
		cur = 1050;
		break;
	case 5:
		cur = 1150;
		break;
	case 6:
		cur = 1350;
		break;
	case 7:
		cur = 1450;
		break;
	default:
		cur = 550;
	}
	return cur;
}

unsigned char sprdchg_psc5415a_cur2reg(uint32_t cur)
{
	unsigned char reg_val;

	if (cur < 500)
		reg_val = 1;
	if ((cur >= 500) && (cur < 650))
		reg_val = 1;
	if ((cur >= 650) && (cur < 800))
		reg_val = 2;
	if ((cur >= 800) && (cur < 960))
		reg_val = 3;
	if ((cur >= 960) && (cur < 1150))
		reg_val = 4;
	if ((cur >= 1150) && (cur < 1250))
		reg_val = 5;
	if ((cur >= 1250) && (cur < 1400))
		reg_val = 6;
	if (cur >= 1400)
		reg_val = 7;
	return reg_val;

}

unsigned char sprdchg_psc5425a_cur2reg(uint32_t cur)
{
	unsigned char reg_val;

	if (cur < 650)
		reg_val = 0; 
	if ((cur >= 650) && (cur < 900))
		reg_val = 1;
	if ((cur >= 900) && (cur < 1100))
		reg_val = 2;
	if ((cur >= 1100) && (cur < 1300))
		reg_val = 3;
	if ((cur >= 1300) && (cur < 1500))
		reg_val = 4;
	if ((cur >= 1500) && (cur < 1700))
		reg_val = 5;
	if ((cur >= 1700) && (cur < 1950))
		reg_val = 6;
	if (cur >= 1950)
		reg_val = 7;
	return reg_val;
}

unsigned int sprdchg_psc5415a_reg2cur(unsigned char reg_val)
{
	unsigned int cur = 0;

	switch (reg_val) {
		case 0:
			cur = 450;
			break;
		case 1:
			cur = 600;
			break;
		case 2:
			cur = 790;
			break;
		case 3:
			cur = 950;
			break;
		case 4:
			cur = 1100;
			break;
		case 5:
			cur = 1200;
			break;
		case 6:
			cur = 1300;
			break;
		case 7:
			cur = 1500;
			break;
		default:
			cur = 450;

	}
	return cur;
}

unsigned int sprdchg_psc5425a_reg2cur(unsigned char reg_val)
{
	unsigned int cur = 0;

	switch (reg_val) {
	case 0:
		cur = 550;
		break;
	case 1:
		cur = 800;
		break;
	case 2:
		cur = 1000;
		break;
	case 3:
		cur = 1200;
		break;
	case 4:
		cur = 1400;
		break;
	case 5:
		cur = 1600;
		break;
	case 6:
		cur = 1800;
		break;
	case 7:
		cur = 2000;
		break;
	default:
		cur = 550;
	}
	return cur;
}

unsigned int sprdchg_tq24157_reg2cur(unsigned char reg_val)
{
	unsigned char vendor_id2 = 0;
	
	vendor_id2 = fan54015_get_vendor_id2();
	if(vendor_id2 == VENDOR_ID2_ETA6937){
	
		return (550 + reg_val * 100);
	}
	else
	{
		return (550 + (reg_val % 8) * 100);
	}
}

unsigned char sprdchg_tq24157_cur2reg(uint32_t cur)
{
	unsigned char reg_val;
	unsigned char vendor_id2 = 0;
	SPRDCHG_DEBUG("%s cur=%d\n", __func__, cur);
	
	vendor_id2 = fan54015_get_vendor_id2();
	if(vendor_id2 == VENDOR_ID2_ETA6937){
		reg_val = (cur - 550) / 100;
	}
	else{
		if (cur < 650)
			reg_val = 0;
		else if (cur >= 1250)
			reg_val = 7;
		else
			reg_val = (cur - 550) / 100;	
	}	

	return reg_val;
}


void sprdchg_fan54015_reset_timer(void)
{
	fan54015_reset_timer();
}

void sprdchg_fan54015_otg_enable(int enable)
{
	fan54015_otg_enable(enable);
}

void sprdchg_fan54015_stop_charging(unsigned int flag)
{
	fan54015_stop_charging();
}

int sprdchg_fan54015_get_charge_status(void)
{
	unsigned char chg_status = fan54015_get_chg_status();

	switch (chg_status) {
	case CHG_READY:
		SPRDCHG_DEBUG("fan54015 charge ready\n");
		return POWER_SUPPLY_STATUS_NOT_CHARGING;
	case CHG_CHGING:
		SPRDCHG_DEBUG("fan54015 is charging\n");
		return POWER_SUPPLY_STATUS_CHARGING;
	case CHG_DONE:
		SPRDCHG_DEBUG("fan54015 charge full\n");
		return POWER_SUPPLY_STATUS_FULL;
	default:
		SPRDCHG_DEBUG("fan54015 charge fault\n");
		return POWER_SUPPLY_STATUS_DISCHARGING;
	}
}

int sprdchg_fan54015_get_charge_fault(void)
{
	unsigned char fault_val = 0;
	int ret = 0;

	fault_val = fan54015_get_fault_val();
	switch (fault_val) {
	case CHG_NO_FAULT:
		SPRDCHG_DEBUG("no fault\n");
		ret = SPRDBAT_CHG_END_NONE_BIT;
		break;
	case CHG_THM_SHUTDOWN:
		SPRDCHG_DEBUG("chg hot\n");
		ret = SPRDBAT_CHG_END_OTP_OVERHEAT_BIT;
		break;
	case CHG_VBAT_OVP:
		SPRDCHG_DEBUG("chg vbat ovp\n");
		ret = SPRDBAT_CHG_END_BAT_OVP_BIT;
		break;
	default:
		SPRDCHG_DEBUG("chg unspec fault\n");
		ret = SPRDBAT_CHG_END_UNSPEC;
	}
	return ret;
}

void sprdchg_fan54015_set_cur(uint32_t cur)
{
	unsigned char reg_val = 0, vendor_id = VENDOR_FAN54015;
	unsigned char vendor_id2 = 0;

	if (NULL == bat_drv_data)
		return;
	SPRDCHG_DEBUG("%s cur1 =%d\n", __func__, cur);
	if (bat_drv_data->bat_info.chg_current_type_limit < cur)
		bat_drv_data->bat_info.chg_current_type_limit = cur;
	SPRDCHG_DEBUG("%s cur =%d\n", __func__, cur);

	vendor_id = fan54015_get_vendor_id();
	vendor_id2 = fan54015_get_vendor_id2();
	if (vendor_id == VENDOR_FAN54015)
		reg_val = sprdchg_fan54015_cur2reg(cur);

	if (vendor_id == VENDOR_TQ24157)
		reg_val = sprdchg_tq24157_cur2reg(cur);

	if (vendor_id == VENDOR_PSC54X5A)
	{
		if(vendor_id2 == VENDOR_ID2_PSC5425A)
		{
			reg_val = sprdchg_psc5425a_cur2reg(cur);
		}
		else
		{
			reg_val = sprdchg_psc5415a_cur2reg(cur);
		}

	}

	SPRDCHG_DEBUG("%s reg_val=%d vendor_id=0x%x vendor_id2=0x%x\n", __func__, reg_val, vendor_id, vendor_id2);
	fan54015_set_chg_current(reg_val);
}

void sprdchg_fan54015_enable_chg(void)
{
	fan54015_enable_chg();
}

unsigned int sprdchg_fan54015_get_chgcur(void)
{
	unsigned char reg_val = 0, vendor_id = VENDOR_FAN54015;
	unsigned int cur = 550;
	unsigned char vendor_id2 = 0;

	reg_val = fan54015_get_chg_current();
	vendor_id = fan54015_get_vendor_id();
	vendor_id2 = fan54015_get_vendor_id2();

	if (vendor_id == VENDOR_FAN54015)
		cur = sprdchg_fan54015_reg2cur((reg_val% 8)) ;

	if (vendor_id == VENDOR_TQ24157)
		cur = sprdchg_tq24157_reg2cur(reg_val);

	if (vendor_id == VENDOR_PSC54X5A)
	{
		if(vendor_id2 == VENDOR_ID2_PSC5425A)
		{
			cur = sprdchg_psc5425a_reg2cur((reg_val % 8));
		}
		else
		{
			cur = sprdchg_psc5415a_reg2cur((reg_val% 8)) ;
		}
	}
		
	SPRDCHG_DEBUG("%s cur =%d\n", __func__, cur);
	return cur;
}

void sprdchg_fan54015_adjust_cur(int adjust)
{
	unsigned char reg_val = fan54015_get_chg_current();
	unsigned char vendor_id2 = 0;
	vendor_id2 = fan54015_get_vendor_id2();
	if(vendor_id2 == VENDOR_ID2_ETA6937){
		if (adjust > 0) {
			if (reg_val == 15)
				return;
			reg_val++;
		} 
		else {
			if (!reg_val)
				return;
			reg_val--;
		}
	}
	else{
		if (adjust > 0) {
			if (reg_val == 7)
				return;
			reg_val++;
		} 
		else {
			if (!reg_val)
				return;
			reg_val--;
		}		
	}
	fan54015_set_chg_current(reg_val);
	fan54015_enable_chg();
}

int sprdchg_fan54015_get_chgcur_step(int cur, int increase)
{
	unsigned char reg_val = fan54015_get_chg_current();
	unsigned char vendor_id = VENDOR_FAN54015;
	unsigned char vendor_id2 = 0;
	int new_cur = 0;
	vendor_id = fan54015_get_vendor_id();
	vendor_id2 = fan54015_get_vendor_id2();
	if(vendor_id2 == VENDOR_ID2_ETA6937){
		if (increase > 0) {
			if (reg_val == 15)
				return 0;
		reg_val++;
		} 
		else {
			if (!reg_val)
				return 0;

			reg_val--;
		}
	}
	else{
		reg_val=reg_val % 8;
		if (increase > 0) {
			if (reg_val == 7)
				return 0;
		reg_val++;
		} 
		else {
			if (!reg_val)
				return 0;

			reg_val--;
		}		
	}

	if (vendor_id == VENDOR_FAN54015)
		new_cur = sprdchg_fan54015_reg2cur(reg_val);

	if (vendor_id == VENDOR_TQ24157)
		new_cur = sprdchg_tq24157_reg2cur(reg_val);

	if (vendor_id == VENDOR_PSC54X5A)
	{
		if(vendor_id2 == VENDOR_ID2_PSC5425A)
		{
			cur = sprdchg_psc5425a_reg2cur(reg_val);
		}
		else
		{
			cur = sprdchg_psc5415a_reg2cur(reg_val);
		}
	}


	if (increase > 0)
		return (new_cur - cur);

	else
		return (cur - new_cur);
}

void sprdchg_fan54015_init(struct sprdbat_drivier_data *bdata)
{
	unsigned char vendor_id;
	if (NULL == bdata)
		return;
	SPRDCHG_DEBUG("vendor_id2 = %x/n",fan54015_get_vendor_id2());
	vendor_id = fan54015_get_vendor_id();
	bat_drv_data = bdata;
	sprdchg_fan54015_set_safety_vol(bat_drv_data->pdata->chg_end_vol_pure);
	sprdchg_fan54015_set_safety_cur(bat_drv_data->pdata->adp_dcp_cur);
	fan54015_init();
	if (vendor_id != VENDOR_PSC54X5A) {
		sprdchg_fan54015_set_vindpm(bat_drv_data->pdata->chg_end_vol_pure);
	}

	if (bat_drv_data->pdata->chg_full_condition == FROM_EXT_IC) {
		fan54015_enable_cur_terminal(1);
		sprdchg_fan54015_termina_cur_set(bat_drv_data->pdata->chg_end_cur);
	} else {
		fan54015_enable_cur_terminal(0);
	}
	sprdchg_fan54015_termina_vol_set(bat_drv_data->pdata->chg_end_vol_pure);
	if (vendor_id == VENDOR_PSC54X5A) {
		sprdchg_fan54015_set_safety_vol(bat_drv_data->pdata->chg_end_vol_pure);
		sprdchg_fan54015_set_safety_cur(bat_drv_data->pdata->adp_dcp_cur);
		//fan54015_enable_cur_terminal(1);
		//sprdchg_fan54015_termina_cur_set(bat_drv_data->pdata->chg_end_cur);
	}
}

struct sprd_ext_ic_operations sprd_54015_op = {
	.ic_init = sprdchg_fan54015_init,
	.charge_start_ext = sprdchg_fan54015_enable_chg,
	.set_charge_cur = sprdchg_fan54015_set_cur,
	.charge_stop_ext = sprdchg_fan54015_stop_charging,
	.get_charging_status = sprdchg_fan54015_get_charge_status,
	.get_charging_fault = sprdchg_fan54015_get_charge_fault,
	.timer_callback_ext = sprdchg_fan54015_reset_timer,
	.otg_charge_ext = sprdchg_fan54015_otg_enable,
	.get_charge_cur_ext = sprdchg_fan54015_get_chgcur,
	.set_termina_cur_ext = sprdchg_fan54015_termina_cur_set,
	.set_termina_vol_ext = sprdchg_fan54015_termina_vol_set,
	.get_chgcur_step = sprdchg_fan54015_get_chgcur_step,
};

struct sprd_ext_ic_operations *sprd_get_54015_ops(void)
{
	return &sprd_54015_op;
}
