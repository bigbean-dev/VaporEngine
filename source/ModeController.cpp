//Brandon Nguyen VaporEngine 2017- ModeController.cpp
#include "ModeController.h"

#include "PongMode.h"

namespace ve
{
	void ModeController::RequestModePush(ModeType modeType, bool replaceTop /*= false*/)
	{
		ModeRequestParameters params;
		params.InitPush(modeType, replaceTop);
		mRequestQueue.push(params);
	}

	void ModeController::RequestModePop(ModeType modeType)
	{
		ModeRequestParameters params;
		params.InitPop(modeType);
		mRequestQueue.push(params);
	}

	void ModeController::ProcessStateChanges()
	{
		//If request queue is empty, don't need to do anything
		if (mRequestQueue.empty())
		{
			return;
		}

		ModeRequestParameters &params = mRequestQueue.front();
		if (params.mIsPush)
		{
			//Push new mode
			ApplyModePush(params.mModeType, params.mReplaceTop);
		}
		else
		{
			//Pop top mode
			ApplyModePop(params.mModeType);
		}

		mRequestQueue.pop();
	}

	ModeRef ModeController::GetActiveMode() const
	{
		if (!mModeStack.empty())
		{
			return mModeStack.top();
		}
		return nullptr;
	}

	ModeRef ModeController::BuildMode(ModeType modeType) const
	{
		ModeRef newModeRef = nullptr;
		switch (modeType)
		{
		case ModeType::Pong:
			newModeRef = std::make_shared<PongMode>();
			break;
		default:
			assert(false && "Invalid ModeType in ModeController::BuildMode");
			break;
		}

		assert(newModeRef);
		return newModeRef;
	}

	void ModeController::ApplyModePush(ModeType modeType, bool replaceTop)
	{
		ModeRef newModeRef = BuildMode(modeType);
		assert(newModeRef);
		if (newModeRef)
		{
			if (!mModeStack.empty())
			{
				ModeRef topModeRef = mModeStack.top();
				assert(topModeRef);
				if (replaceTop)
				{
					mModeStack.pop();
				}
				else if(topModeRef)
				{
					topModeRef->Pause();
				}
			}

			newModeRef->Init();
			mModeStack.push(newModeRef);
		}
	}

	void ModeController::ApplyModePop(ModeType modeType)
	{
		assert(!mModeStack.empty());
		if (mModeStack.empty())
		{
			return;
		}

		bool popTopMode = true;
		assert(mModeStack.top());
		if (mModeStack.top())
		{
			//Ensure top mode is the mode requested to close
			ModeType topModeType = mModeStack.top()->GetModeType();
			assert(modeType == topModeType);
			if (modeType != topModeType)
			{
				popTopMode = false;
			}
		}

		if (popTopMode)
		{
			mModeStack.pop();

			assert(mModeStack.top());
			if (mModeStack.top())
			{
				mModeStack.top()->Resume();
			}
		}
	}

	void ModeController::ModeRequestParameters::InitPush(ModeType modeType, bool replaceTop)
	{
		mModeType = modeType;
		mIsPush = true;
		mReplaceTop = replaceTop;
	}

	void ModeController::ModeRequestParameters::InitPop(ModeType modeType)
	{
		mModeType = modeType;
		mIsPush = false;
	}
}