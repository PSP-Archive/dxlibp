DX Library Portable	Ver0.5.7_UTF8
�J���ҁF�J�� & �L�R�L�@

�����PSP�̊J�����ȒP�ɍs�����߂̃��C�u�����ł��B
�ڂ������̓z�[���y�[�W��Wiki���������Ă��������B
http://dxlibp.sourceforge.jp/
�Ȃ��A���̃o�[�W�����͏�����ɂ��\�[�X�����J�ł��܂���B

�y���ӁI�z
���̃��C�u�������g�p���ăQ�[�����쐬�����ꍇ�A���̒��쌠�\����K���܂߂Ă��������B

�y���쌠�\���K�{���ځz

zlib
Copyright (c) 1995-2004 Jean-loup Gailly and Mark Adler.
��������LoadGraph���̉摜�ǂݍ��݊֐����g���Ă��Ȃ��ꍇ�͖����ő��v�ł��B

intraFont
Uses intraFont by BenHur
��������DrawString���̕�����`��֐����g���Ă��Ȃ��ꍇ�͖����ő��v�ł��B

liblzr
Uses liblzr by BenHur


�S�p�����\�����C�u����
���̃\�t�g�E�F�A��mediumgauge���쐬�̑S�p�����\�����C�u�������g�p���Ă��܂��B
��������printfDx���̊ȈՕ�����`��֐����g���Ă��Ȃ��ꍇ�͖����ő��v�ł��B

�y�K�{����Ȃ����Ǔ���Ă��ꂽ����������ځz

DX���C�u����Portable
DX Library Portable Copyright (C) 2008-2010 Kawai Yuichi.

�yDX���C�u�����Ƃ̎�ȈႢ�z
PSP�œ���
�摜�t�H�[�}�b�g�͌����_��PNG�̂�
�����t�H�[�}�b�g�͌����_��MP3�̂�
�t�H���g��pgf�t�H���g�̂�
�����Đ��\�ȉ����f�[�^�͂W�܂�
��ʃT�C�Y��480x272�Œ�
�ŏ����痠��ʂ��g���ݒ�ɂȂ��Ă���
GPU���g�킸�ɕ`�悷��@�\�͖���
�������̊֐�������Ȃ�ɂ���
�A�[�J�C�u�ǂݍ��݋@�\������
3D�@�\������

�y0.4.x�ȑO����̍X�V���ӁI�z
�ˑ����Ă��郉�C�u�������ύX���ꂽ�̂ŁAMakefile�Ɏw�肷��LIBS�̍��ڂ��ω����Ă��܂��B
LIBS = dxlibp.a -lpspgu -lz -lm -lpsprtc -lpspaudio -lpspaudiocodec -lstdc++ -lpsputility -lpspvalloc -lpsppower
�܂��A����ɔ����ĕK�v�Ȓ��쌠�\�����ύX����Ă��܂��B��ɂ���̂Ŋm�F���Ă��������B

VFPU���g���悤�ɂȂ����̂ŁA�X���b�h�A�g���r���[�g�ݒ�����L�̂悤�ɏC�����Ă��������B
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

VFPU���g���l��
���������֐��A�Z�p�x���֐��ADra��ModiGraph�ADrawRotaGraph�y�т��̔h���֐��ł͓����ňꎞ�I��VFPU���W�X�^�̂O�`�Q�u���b�N���g���Ă��܂��B
�����̊֐����Ă΂ꂽ���_�ŊY������VFPU���W�X�^�̓��e���j�󂳂�܂��̂ŁA���ӂ��Ă��������B
�܂��A����̃A�b�v�f�[�g��VFPU���g���֐�����������A�g�����W�X�^�𑝂₷�\��������܂��B


�y�X�V�����z

Ver0.0.0		���J�J�n
Ver0.0.1		�p���������o�O���C��
Ver0.0.2		���������o�O�C��
Ver0.1.0		Slice�̓����ADeleteGraph�֐������Y��Ă����̂Œǉ��ATransFlag���g����悤�ɂ����AConvertGraphFormat�֐���ǉ��A���̑��o�O�C��
Ver0.1.1		DrawRotaGraph�̌v�Z�ɒp���������~�X���������̂ŏC���B�S�Ẳ摜�`��֐���Slice�𗘗p����悤�ɕύX
Ver0.2.0		MP3�t�@�C����PlayMusic�ōĐ��ł���悤�ɂ��܂����BWAV�͍Đ��ł��Ȃ��̂Œ��ӂł�
Ver0.2.1		printfDx�n�֐���S�ʏ����������܂����BDrawString_Shinonome�֐���ǉ����܂����B
Ver0.3.0		GetTextOSK�֐���ǉ����܂����B
Ver0.3.1		�t�@�C���̎�舵���Ɋւ���v���I�ȃo�O���C��
Ver0.3.2		libjpeg��PSP������Jpeg�f�R�[�_�̂ǂ�����g������DXP�R���p�C�����ɑI���\�ɂ����B
Ver0.4.0		intraFont�����APSP�{�̂ɑg�ݍ��܂�Ă���t�H���g�𗘗p�\�ɂ����B���y�Đ��@�\�̍č\�z�������B
Ver0.4.1		DrawExtendGraph�ŕ\�����ʂ����]���鎞��DrawTurnGraph�g�p���ŕ`�挋�ʂ����������Ȃ�����C���B�i�O�L�ɊY������ꍇ�͓����I��DrawModiGraph���g�����ߎ኱�̑��x�ቺ���������܂��B
Ver0.4.2		GetAnalogInput���s���Ȓl��Ԃ������C���B
Ver0.4.3		LoadDivGraph�֐���DerivationGraph�֐��������B
Ver0.4.4		�����\���̈�V��SetDrawScreen�֐��̒ǉ��BSetDrawScreen�ɓn���O���t�B�b�N�n���h����VRAM��ɂ���K�v������܂��B
Ver0.4.5		���y�Đ��@�\�̃o�O�C���i�V���[�e�B���O��D�����ɂ��\�[�X�R�[�h���C�j�A�`�惋�[�`���̍������Ȃǂł��B
Ver0.4.6		���傤���Ȃ��~�X�^�C�v�ŕ`�挋�ʂ����������Ȃ��Ă����̂��C��
Ver0.4.7
Ver0.4.8		�o�O�C��
Ver0.4.9		�u�����h���[�h�̎d�l��{�Ƃɍ��킹���B
Ver0.4.10		���Z����������ɓ����Ȃ��o�O���C��
Ver0.4.11		�p���b�g�`����PNG�摜�̓p���b�g���g���ĕ`�悷��悤�ɏC��
Ver0.4.12		MP3�̍Đ������艻�A������`��̃o�O�C���A���̑����낢��
Ver0.4.14		�`��@�\���X�ɍ�����
Ver0.5.0		�\�[�X�R�[�h�S�̂��قƂ�Ǐ����������B
Ver0.5.1		�o�O�C��
Ver0.5.2		�o�O�C��
Ver0.5.3		�o�O�C��
Ver0.5.4		�o�O�C��
Ver0.5.5		�o�O�C��
Ver0.5.6		�o�O�C��
Ver0.5.7		�o�O�C��
Ver0.5.7_UTF8		�C���g���t�H���g�ŗ��p���镶���R�[�h���V�t�gJIS����UTF8�֕ύX
